#include "VoxelMap.h"
#include "Log.h"
#include <glm/gtx/norm.hpp>

Voxel &Brick::at(std::uint16_t index) { return voxels.at(index); }

Brick &Sector::at(std::uint8_t index) {
    std::uint8_t slot = brick_indices.at(index);
    if (slot != invalid_index) {
        return bricks.at(slot);
    }
    brick_indices.at(index) = bricks.size();
    return bricks.emplace_back();
}

bool Sector::exists(std::uint8_t index) {
    return brick_indices.at(index) != invalid_index;
}

Sector::Sector() { brick_indices.fill(invalid_index); }

Sector &VoxelMap::get_sector(const glm::ivec3 &pos) {
    // TODO: Initialize using some kind of TerrainGenerator
    return sectors[pos];
}

Brick &VoxelMap::get_brick(const glm::ivec3 &pos) {
    Sector &sector = get_sector(pos >> SectorIndexer::shift);
    return sector.at(SectorIndexer::to_index(pos));
}

Voxel VoxelMap::get(const glm::ivec3 &pos) {
    Brick &brick = get_brick(pos >> BrickIndexer::shift);
    return brick.at(BrickIndexer::to_index(pos));
}

void VoxelMap::set(const glm::ivec3 &pos, Voxel voxel) {
    Brick &brick = get_brick(pos >> BrickIndexer::shift);
    brick.at(BrickIndexer::to_index(pos)) = voxel;
}

int VoxelMap::step_level(const glm::ivec3 &pos) {
    int k = SectorIndexer::shift + BrickIndexer::shift;
    auto result = sectors.find(pos >> k);

    if (result == sectors.end()) {
        return k;
    }

    k = BrickIndexer::shift;
    int index = SectorIndexer::to_index(pos >> k);
    if (!result->second.exists(index)) {
        return k;
    }

    Brick &brick = result->second.at(index);

    return brick.at(BrickIndexer::to_index(pos)).isAir() ? 0 : -1;
}

std::optional<HitResult>
    VoxelMap::raycast(glm::dvec3 origin, glm::dvec3 dir, double max_distance) {

    // Let pos be current voxel position of ray
    // if pos is x than ray_i in [ pos_i, pos_i + 1),
    // so next voxel boundary is pos + step.
    //
    // Ray is given by P(t) = origin + t * dir
    //
    // target = pos + step
    // Find ts such that P(ts_i)_i = target_i.
    // origin_i + ts_i * dir_i = target_i
    // ts_i = (target_i - origin_i) / dir_i
    // ts = (target - origin) / dir
    //
    // Grab smallest t = min(ts_1, ...)
    //
    // P(t) is now exactly at boundary of new voxel
    // For floating point error we add 0.0001, so it crosses the boundary
    //
    // hit_pos = P(t) = origin + t * dir
    // pos = floor(hit_pos)
    //
    // Optimizations:
    // - ts = (target - origin) / dir = pos / dir + step / dir - origin / dir.
    //   We can precompute (step - origin) / dir = t_start.
    //   Now ts = pos / dir + t_start
    //
    // - Multiplication is faster than division, so precompute inv_dir = 1/dir

    glm::dvec3 inv_dir = 1.0 / dir;
    glm::ivec3 pos = glm::floor(origin);
    glm::dvec3 t_start = (glm::step(0.0, dir) - origin) * inv_dir;

    double max_distance_sq = max_distance * max_distance;
    double t_max = max_distance / glm::length(dir);

    bool within_distance = true;
    while (within_distance) {
        glm::dvec3 ts;
        ts.x = dir.x == 0.0 ? std::numeric_limits<double>::infinity()
                            : t_start.x + pos.x * inv_dir.x;
        ts.y = dir.y == 0.0 ? std::numeric_limits<double>::infinity()
                            : t_start.y + pos.y * inv_dir.y;
        ts.z = dir.z == 0.0 ? std::numeric_limits<double>::infinity()
                            : t_start.z + pos.z * inv_dir.z;

        double t = std::min({ts.x, ts.y, ts.z}) + 0.0001;

        if (t > t_max) {
            within_distance = false;
            t = t_max;
        }

        glm::dvec3 hit_pos = origin + t * dir;
        pos = glm::floor(hit_pos);

        int k = step_level(pos);
        if (k < 0) {
            glm::bvec3 side_mask = glm::greaterThanEqual(glm::dvec3(t), ts);

            return HitResult{
                .pos = pos,
                .uv = glm::fract(
                    glm::mix(
                        glm::vec2(hit_pos.x, hit_pos.z), glm::vec2(hit_pos.y),
                        glm::vec2(side_mask.x, side_mask.z)
                    )
                ),
                .normal = glm::mix(glm::dvec3(0), -glm::sign(dir), side_mask),
            };
        }

        int m = (1 << k) - 1;
        pos.x = dir.x < 0 ? (pos.x & ~m) : (pos.x | m);
        pos.y = dir.y < 0 ? (pos.y & ~m) : (pos.y | m);
        pos.z = dir.z < 0 ? (pos.z & ~m) : (pos.z | m);
    }

    return std::nullopt;
}
