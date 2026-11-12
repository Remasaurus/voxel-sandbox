#include "VoxelMap.h"

Voxel& Brick::at(std::uint16_t index) {
    return voxels.at(index);
}

Brick& Sector::at(std::uint8_t index) {
    std::uint8_t slot = brick_indices.at(index);
    if (slot != invalid_index) {
        return bricks.at(slot);
    }
    brick_indices.at(index) = bricks.size();
    return bricks.emplace_back();
}

Sector::Sector() {
    brick_indices.fill(invalid_index);
}

Sector& VoxelMap::get_sector(const glm::ivec3& pos) {
    // TODO: Initialize using some kind of TerrainGenerator
    return sectors[pos];
}

Brick& VoxelMap::get_brick(const glm::ivec3& pos) {
    Sector& sector = get_sector(pos >> SectorIndexer::shift);
    return sector.at(SectorIndexer::to_index(pos));
}

Voxel VoxelMap::get(const glm::ivec3& pos) {
    Brick& brick = get_brick(pos >> BrickIndexer::shift);
    return brick.at(BrickIndexer::to_index(pos));
}

void VoxelMap::set(const glm::ivec3& pos, Voxel voxel) {
    Brick& brick = get_brick(pos >> BrickIndexer::shift);
    brick.at(BrickIndexer::to_index(pos)) = voxel;
}

std::optional<HitResult> VoxelMap::raycast(glm::dvec3 origin, glm::dvec3 dir, double max_distance) {
    glm::dvec3 inv_dir = 1.0 / dir;
    glm::dvec3 t_start = (glm::step(0.0, dir) - origin) * inv_dir;

 
    (void)inv_dir;
    (void)t_start;
    (void)max_distance;

    return std::nullopt;
}
