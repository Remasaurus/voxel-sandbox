#include "VoxFileParser.h"
#include "../Log.h"
#include "Voxel.h"
#include <cstdint>
#include <cstring>
#include <fstream>
#include <glm/fwd.hpp>
#include <iostream>
#include <memory>
#include <string_view>
#include <variant>

namespace {
template <typename T>
T load(const std::vector<std::byte> &file_buffer, size_t *offset) {
    T var;
    std::memcpy(&var, file_buffer.data() + *offset, sizeof(T));
    *offset += sizeof(T);
    return var;
}

// because the nodes may not be in the file in the node_id order
void insert(std::vector<std::variant<TransformNode,GroupNode,ShapeNode>>* node_vec, std::variant<TransformNode,GroupNode,ShapeNode>&& node) {
    auto get_id = [](const auto &n) { return n.node_id; };
    int32_t node_id = std::visit(get_id, node);
    if(node_id >= static_cast<int32_t>(node_vec->size())) {
        node_vec->resize(static_cast<size_t>(node_id + 1));
    }

    (*node_vec)[static_cast<size_t>(node_id)] = std::move(node);
}

// identity = 4
glm::imat3x3 decode_rotation_matrix(uint8_t row_major_rotation){
    uint8_t i_of_non_zero_entry_1st_row = row_major_rotation & 0x3;
    uint8_t i_of_non_zero_entry_2nd_row = (row_major_rotation >> 2) & 0x3;
    uint8_t i_of_non_zero_entry_3rd_row = 3 - i_of_non_zero_entry_1st_row - i_of_non_zero_entry_2nd_row;
    bool sign_1st_row = row_major_rotation & 0x10;
    bool sign_2nd_row = row_major_rotation & 0x20;
    bool sign_3rd_row = row_major_rotation & 0x40;
    
    int8_t val0 = 1 * (sign_1st_row ? -1 : 1);
    int8_t val1 = 1 * (sign_2nd_row ? -1 : 1);
    int8_t val2 = 1 * (sign_3rd_row ? -1 : 1);

    glm::imat3x3 output(0);
    output[i_of_non_zero_entry_1st_row][0] = val0;
    output[i_of_non_zero_entry_2nd_row][1] = val1;
    output[i_of_non_zero_entry_3rd_row][2] = val2;

    return output;
}

std::vector<VoxVoxel>
    parse_voxels(size_t offset, const std::vector<std::byte> &file_buffer) {
    uint32_t n_voxels = load<uint32_t>(file_buffer, &offset);
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

std::string parse_string_type(const std::vector<std::byte> &file_buffer, size_t *offset) {
    int32_t buffer_size = load<int32_t>(file_buffer, offset);

    std::string output{};
    size_t buffer_size_t = static_cast<size_t>(buffer_size);
    output.resize(buffer_size_t);
    std::memcpy(output.data(), file_buffer.data() + *offset, buffer_size_t);
    *offset += buffer_size_t;
    return output;
}

std::unordered_map<std::string, std::string> parse_dict_type(const std::vector<std::byte> &file_buffer, size_t *offset) {
    std::unordered_map<std::string, std::string> output{};
    uint32_t n_key_value_pairs{};
    std::memcpy(&n_key_value_pairs, file_buffer.data() + *offset, sizeof(uint32_t));
    *offset += sizeof(uint32_t);

    for (size_t i = 0; i < n_key_value_pairs; i++) {
        auto key = parse_string_type(file_buffer, offset);
        auto value = parse_string_type(file_buffer, offset);
        output[key] = value;
    }
    return output;
}


GroupNode parse_group_node(const std::vector<std::byte> &file_buffer, size_t offset) {

    int32_t node_id = load<int32_t>(file_buffer, &offset);
    auto node_attributes = parse_dict_type(file_buffer, &offset);
    int32_t n_children = load<int32_t>(file_buffer, &offset);

    std::vector<int32_t> child_node_ids;
    child_node_ids.reserve(static_cast<size_t>(n_children));
    for (size_t i = 0; i < static_cast<size_t>(n_children); i++) {
        child_node_ids.push_back(load<int32_t>(file_buffer, &offset));
    }

    return {node_id, node_attributes, n_children, child_node_ids};
}

ShapeNode parse_shape_node(const std::vector<std::byte> &file_buffer, size_t offset) {
    int32_t node_id = load<int32_t>(file_buffer, &offset);
    auto node_attributes = parse_dict_type(file_buffer, &offset);
    int32_t n_models = load<int32_t>(file_buffer, &offset);

    std::vector<ShapeModel> models;
    models.reserve(static_cast<size_t>(n_models));
    for (size_t i = 0; i < static_cast<size_t>(n_models); i++) {
        ShapeModel model;
        model.model_id = load<int32_t>(file_buffer, &offset);
        auto model_attributes = parse_dict_type(file_buffer, &offset);
        if(auto it = model_attributes.find("_f"); it != model_attributes.end()) {
            model.frame_index = std::stoi(it->second);
        }
        models.emplace_back(model);
    }
    return {node_id, node_attributes, n_models, models};
}

TransformNode parse_transform_node(const std::vector<std::byte> &file_buffer, size_t offset) {
    int32_t node_id = load<int32_t>(file_buffer, &offset);
    auto node_attributes = parse_dict_type(file_buffer, &offset);

    int32_t child_node_id = load<int32_t>(file_buffer, &offset);
    offset += (sizeof(int32_t)); // skiping the reserved_id = -1
    
    int32_t layer_id = load<int32_t>(file_buffer, &offset);
    int32_t n_frames = load<int32_t>(file_buffer, &offset);
   
    std::vector<Frame> frames;
    for(size_t i = 0; i < static_cast<size_t>(n_frames); i++) {
        Frame frame;

        auto frame_attributes = parse_dict_type(file_buffer, &offset);
        if(auto it = frame_attributes.find("_r"); it != frame_attributes.end()){
            frame.rotation = std::stoi(it->second);
        }
        if(auto it = frame_attributes.find("_t"); it != frame_attributes.end()){
            auto transform_string = std::stringstream(it->second);
            std::string s;
            for(size_t j = 0; j < frame.translation.size(); j++) {
                std::getline(transform_string, s, ' ');
                frame.translation[j] = std::stoi(s);
            }
        }
        if(auto it = frame_attributes.find("_f"); it != frame_attributes.end()){
            frame.frame_index = std::stoi(it->second);
        }
        frames.push_back(frame);
    }

    return {node_id, node_attributes, child_node_id, -1, layer_id, n_frames, frames};
}
struct InterVoxRepr {
    glm::ivec3 vox_pos;
    uint8_t color_index;
};

// TODO do i need to return dimensions? 
std::vector<Voxel> convert_to_flat_3d_voxel_vec(const std::vector<InterVoxRepr> intervox_vec, const std::array<Voxel, 256> &voxel_palette) {
    glm::ivec3 min{INT32_MAX};
    glm::ivec3 max{INT32_MIN};
    
    for(const auto& inter_vox : intervox_vec) {
        min = glm::min(min,inter_vox.vox_pos);
        max = glm::max(max,inter_vox.vox_pos);
    }
    glm::ivec3 dimensions = (max - min) + 1;
    size_t size_flat_3d_vec = static_cast<size_t>(dimensions.x * dimensions.y * dimensions.z);
    std::vector<Voxel> output(size_flat_3d_vec);

    for(const auto& inter_vox : intervox_vec) {
        glm::ivec3 normalized_vox_pos = inter_vox.vox_pos - min;
        int vox_pos_x = normalized_vox_pos.x;
        int vox_pos_y = normalized_vox_pos.y;
        int vox_pos_z = normalized_vox_pos.z;
        int width = dimensions.x;
        int height = dimensions.y;
        
        size_t index = static_cast<size_t>(vox_pos_x + vox_pos_y * width + vox_pos_z  * (width * height));
        output[index] = voxel_palette[inter_vox.color_index];
    }
    return output;
}

//https://en.cppreference.com/cpp/utility/variant/visit
template<class... Ts>
struct overloads : Ts... { using Ts::operator()...; };
using var_t = std::variant<TransformNode, GroupNode, ShapeNode>;

void DFS(
    const std::vector<std::variant<TransformNode, GroupNode, ShapeNode>> &nodes,
    std::vector<Model> &models, const Transformation &transf, int32_t node_id,
    std::vector<InterVoxRepr> *output
) {
    const var_t &node = nodes[static_cast<size_t>(node_id)];
    const auto visitor = overloads
    {
        [&](const TransformNode &n) {
            //accumulation of transformations
            const auto& frame = n.frames[0]; // always using frame 0 we do not support animations
            Transformation new_transf;

            new_transf.rotation = transf.rotation * decode_rotation_matrix(static_cast<uint8_t>(frame.rotation));
            glm::ivec3 new_translation(frame.translation[0], frame.translation[1], frame.translation[2]);
            new_transf.translation = transf.rotation * new_translation + transf.translation;

            DFS(nodes, models, new_transf, n.child_node_id, output);
        },
        [&](const GroupNode &n){ 
            for(const int32_t child_node_id : n.child_node_ids) {
                DFS(nodes, models, transf, child_node_id, output);
            }
        },
        [&](const ShapeNode &n){ 
            auto voxel_model = n.models[0].model_id; //always using first model we do not support animations
            Model& model = models[static_cast<size_t>(voxel_model)];
            glm::ivec3 center{model.bounds.x/2, model.bounds.y/2, model.bounds.z/2};
            for(const auto& voxel : model.voxels) {
                glm::ivec3 vox_pos{voxel.x, voxel.y, voxel.z};
                glm::ivec3 relative_vox_pos = vox_pos - center;
                glm::ivec3 new_vox_pos = transf.rotation * relative_vox_pos + transf.translation;
                output->push_back({new_vox_pos, voxel.color_index});
            };
        }
    };
    std::visit(visitor, node);
}

} // namespace

std::expected<std::vector<Voxel>, parse_error>
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

    std::vector<Model> models{};
    std::vector<std::variant<TransformNode, GroupNode, ShapeNode>> nodes{};

    while (offset < total_n_bytes_in_file) {
        const Chunk *chunk =
            std::start_lifetime_as<Chunk>(file_buffer.data() + offset);
        chunk_id = std::string_view(chunk->chunk_id, sizeof(chunk->chunk_id));

        if (chunk_id == "PACK") {
            uint32_t n_models = *std::start_lifetime_as<uint32_t>(
                file_buffer.data() + (offset + sizeof(Chunk))
            );
            models.reserve(n_models);
        } else if (chunk_id == "SIZE") {
            auto model_size = *std::start_lifetime_as<Size>(
                file_buffer.data() + (offset + sizeof(Chunk))
            );
            models.emplace_back(Model{.voxels = {}, .bounds = model_size});
        } else if (chunk_id == "XYZI") {
            models.back().voxels =
                parse_voxels(offset + sizeof(Chunk), file_buffer);
                auto& model_size = models.back().bounds;
                size_t size = model_size.x * model_size.y * model_size.z;
                //TODO WIP
                parse_voxels_new(offset + sizeof(Chunk), file_buffer, voxel_palette, size, model_size.x, model_size.y);

        } else if (chunk_id == "RGBA") {
            Log::info("File has custom RGBA pallet (ignored)");
        } else if (chunk_id == "nTRN") {
            insert(&nodes,parse_transform_node(file_buffer, offset + sizeof(Chunk)));
        } else if (chunk_id == "nGRP") {
            insert(&nodes,parse_group_node(file_buffer, offset + sizeof(Chunk)));
        } else if (chunk_id == "nSHP") {
            insert(&nodes, parse_shape_node(file_buffer, offset + sizeof(Chunk)));
        }
        offset +=
            sizeof(Chunk) + chunk->n_bytes_chunk + chunk->n_bytes_children;
    }
    if (nodes.empty()) {
        // TODO just combine the models? 
    }

    std::vector<InterVoxRepr> internal_voxels;
    Transformation transf;
    DFS(nodes, models, transf, 0, &internal_voxels);
    
    return convert_to_flat_3d_voxel_vec(internal_voxels, voxel_palette);
}
