#include "VoxFileParser.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <string_view>

namespace {

std::vector<VoxVoxel>
    parse_voxels(size_t offset, const std::vector<std::byte> &file_buffer) {
    uint32_t n_voxels{};
    std::memcpy(&n_voxels, file_buffer.data() + offset, sizeof(uint32_t));

    offset += sizeof(uint32_t);
    std::vector<VoxVoxel> output;
    output.reserve(n_voxels);
    for (size_t i = 0; i < n_voxels; i++) {
        size_t vox_offset = offset + i * sizeof(VoxVoxel);
        output.emplace_back(
            *std::start_lifetime_as<VoxVoxel>(file_buffer.data() + vox_offset)
        );
    }
    return output;
}

std::vector<Voxel> parse_voxels_new(
    size_t offset, const std::vector<std::byte> &file_buffer,
    const std::array<Voxel, 256> &voxel_palette, size_t size, size_t width, size_t height
) {
    uint32_t n_voxels{};
    std::memcpy(&n_voxels, file_buffer.data() + offset, sizeof(uint32_t));

    offset += sizeof(uint32_t);
    std::vector<Voxel> output{size, Voxel{}};

    for (size_t i = 0; i < n_voxels; i++) {
        size_t vox_offset = offset + i * sizeof(VoxVoxel);
        VoxVoxel vox_voxel {};
        std::memcpy(&vox_voxel, file_buffer.data() + vox_offset, sizeof(VoxVoxel));
        std::pair<Voxel, size_t> voxel_index_pair = vox_voxel.to_voxel(voxel_palette,  width, height);

        output[voxel_index_pair.second] = voxel_index_pair.first;
    }

    return output;
}

} // namespace

std::expected<std::vector<Model>, parse_error>
    parse_vox_file(std::filesystem::path file_path, const std::array<Voxel, 256>& voxel_palette) {
    if (file_path.extension() != ".vox") {
        Log::error(
            "expected .vox file, found: {}", file_path.extension().string()
        );
        return std::unexpected(parse_error::not_a_vox_file);
    }

    std::ifstream file{file_path, std::ios::binary};
    if (!file) {
        Log::error("Could not open file: {}", file_path.string());
        return std::unexpected(parse_error::could_not_open_file);
    }

    const uintmax_t file_size = std::filesystem::file_size(file_path);
    std::vector<std::byte> file_buffer(file_size);
    file.read(
        reinterpret_cast<char *>(file_buffer.data()),
        static_cast<long>(file_size)
    );
    file.close();

    const Header *header = std::start_lifetime_as<Header>(file_buffer.data());
    auto magic = std::string_view(header->magic, sizeof(header->magic));
    if (magic != "VOX ") {
        Log::error("File is not a VOX file, magic: {}", magic);
        return std::unexpected(parse_error::not_a_vox_file);
    }
    size_t offset = sizeof(Header);
    // TODO LOG vox file version
    const Chunk *main_chunk =
        std::start_lifetime_as<Chunk>(file_buffer.data() + offset);
    auto chunk_id =
        std::string_view(main_chunk->chunk_id, sizeof(main_chunk->chunk_id));
    if (chunk_id != "MAIN") {
        Log::error("File does not start with MAIN chunk");
        return std::unexpected(parse_error::does_not_start_with_main_chunk);
    }
    if (main_chunk->n_bytes_chunk != 0) {
        Log::error("Main chunk has bytes");
        return std::unexpected(parse_error::main_chunk_has_bytes);
    }
    offset += sizeof(Chunk);
    const size_t total_n_bytes_in_file = main_chunk->n_bytes_children + offset;

    std::vector<Model> output{};
    while (offset < total_n_bytes_in_file) {
        const Chunk *chunk =
            std::start_lifetime_as<Chunk>(file_buffer.data() + offset);
        chunk_id = std::string_view(chunk->chunk_id, sizeof(chunk->chunk_id));

        if (chunk_id == "PACK") {
            uint32_t n_models = *std::start_lifetime_as<uint32_t>(
                file_buffer.data() + (offset + sizeof(Chunk))
            );
            output.reserve(n_models);
        } else if (chunk_id == "SIZE") {
            auto model_size = *std::start_lifetime_as<Size>(
                file_buffer.data() + (offset + sizeof(Chunk))
            );
            output.emplace_back(Model{.voxels = {}, .bounds = model_size});
        } else if (chunk_id == "XYZI") {
            output.back().voxels =
                parse_voxels(offset + sizeof(Chunk), file_buffer);
                auto& model_size = output.back().bounds;
                size_t size = model_size.x * model_size.y * model_size.z;
                //TODO WIP
                parse_voxels_new(offset + sizeof(Chunk), file_buffer, voxel_palette, size, model_size.x, model_size.y);

        } else if (chunk_id == "RGBA") {
            Log::info("File has custom RGBA pallet (ignored)");
        }

        offset +=
            sizeof(Chunk) + chunk->n_bytes_chunk + chunk->n_bytes_children;
    }

    return output;
}
