#include "Tree64.h"
#include <cstdint>
#include <glm/fwd.hpp>
#include <unordered_map>
#include <vector>

enum class VoxelType : std::uint8_t {
    powder,
    liquid,
    gas,
    fire,
};

struct Voxel {
    VoxelType type;
};

struct VoxelChunk {
    Tree64 root;

    std::vector<Voxel> voxels;
};

class VoxelMap {
public:
    static constexpr std::uint32_t CHUNK_SIZE = 256;

    void sync();

private:
    std::vector<VoxelChunk> chunks;
};

void generate_terrain();
