#include "TerrainGenerator.h"
#include "VoxelMap.h"

Brick TerrainGenerator::get_brick(const glm::ivec3& pos) {
    (void)pos;
    return Brick{};
}
Sector TerrainGenerator::get_sector(const glm::ivec3& pos) {
    (void)pos;
    return Sector{};
}
