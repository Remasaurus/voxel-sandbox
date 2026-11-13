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
    glm::dvec3 inv_dir = 1.0 / dir;
    // P(t) = origin + t * dir
    // t = (P(t) - origin) / dir
    // t_start * dir = vec(origin to step)
    glm::dvec3 t_start = (glm::step(0.0, dir) - origin) * inv_dir;
    glm::ivec3 pos = glm::floor(origin);
    double max_distance_sq = max_distance * max_distance;

    glm::dvec3 max_pos = origin + glm::normalize(dir) * max_distance;

    // origin + t * dir = pos + step
    // t = (pos - origin + step) / dir
    // t = (step - origin) / dir + pos / dir
    // t = t_start + pos / dir

    bool within_distance = true;
    while (within_distance) {
        // pos / dir
        glm::dvec3 nearest = t_start + glm::dvec3(pos) * inv_dir;
        nearest.x = dir.x == 0.0 ? std::numeric_limits<double>::infinity()
                                 : t_start.x + pos.x * inv_dir.x;
        nearest.y = dir.y == 0.0 ? std::numeric_limits<double>::infinity()
                                 : t_start.y + pos.y * inv_dir.y;
        nearest.z = dir.z == 0.0 ? std::numeric_limits<double>::infinity()
                                 : t_start.z + pos.z * inv_dir.z;

        double min =
            std::min(std::min(nearest.x, nearest.y), nearest.z) + 0.0001;

        glm::dvec3 hit_pos = origin + min * dir;

        if (hit_pos.x > max_pos.x || hit_pos.y > max_pos.y ||
            hit_pos.z > max_pos.z) {
            within_distance = false;
            pos = glm::floor(max_pos);
        } else {
            pos = glm::floor(hit_pos);
        }

        int k = step_level(pos);

        // Log::info("Pos: {}, {}, {}", pos.x, pos.y, pos.z);
        // Log::info("hit_pos: {}, {}, {}", hit_pos.x, hit_pos.y, hit_pos.z);
        // Log::info("origin: {}, {}, {}", origin.x, origin.y, origin.z);
        // Log::info("distance: {}", glm::dot(origin, hit_pos));

        if (k < 0) {
            glm::bvec3 side_mask =
                glm::greaterThanEqual(glm::dvec3(min), nearest);

            Log::info("hit");

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
