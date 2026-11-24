#include "TerrainGenerator.h"
#include "VoxelMap.h"

Brick TerrainGenerator::get_brick(const glm::ivec3& pos) {
    glm::ivec3 voxel_min = pos << BrickIndexer::shift;
    Brick output_brick;
    if(voxel_min.y > ground_y) {
        output_brick.voxels.fill({0});
    } else {
        fill_ground(&output_brick, voxel_min);
    }

    return output_brick;
}
Sector TerrainGenerator::get_sector(const glm::ivec3& pos) {
    (void)pos;
    return Sector{};
}
void TerrainGenerator::fill_ground(Brick *brick, const glm::ivec3& voxel_min) {
    glm::ivec3 voxel_max = voxel_min | BrickIndexer::mask;
    if(voxel_max.y < ground_y) {
        brick->voxels.fill({1});
        return;
    }
    for (int z = voxel_min.z; z <= voxel_max.z; z++)
        for (int y = voxel_min.y; y < voxel_max.y; y++) {
            if (y > ground_y) {
                break;
            }
            for (int x = voxel_min.x; x < voxel_max.x; x++) {
                brick->voxels[BrickIndexer::to_index(x, y, z)] = {1};
            }
        }
}
