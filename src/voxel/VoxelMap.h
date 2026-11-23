#pragma once

// VoxelMap is based on the BrickMap: https://studenttheses.uu.nl/handle/20.500.12932/20460
//
// Inspirations:
// - Comparison different structures: https://github.com/dubiousconst282/VoxelRT/tree/alt-renderers
// - Brickmap implementation: https://github.com/stijnherfst/BrickMap/tree/master

#include <concepts>
#include <cstdint>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL // Needed for hashing glm::ivec3
#include <glm/gtx/hash.hpp>
#include <optional>
#include <vector>

#include "TerrainGenerator.h"
#include "Voxel.h"

template <size_t Shift>
struct BitIndexer {

    static constexpr int shift = Shift;
    static constexpr int mask = (1 << shift) - 1;
    static constexpr int max_length = 1 << shift;
    static constexpr int max_area = max_length * max_length;
    static constexpr int max_volume = max_area * max_length;

    template <typename T>
    static constexpr T to_index(T x, T y, T z) {
        // x + y * max_length + z * max_area but with correct bitshifting
        return (x & mask) | ((y & mask) << shift) | ((z & mask) << shift * 2);
    }

    template <typename T>
    static constexpr T to_index(glm::vec<3, T> vec) {
        return to_index<T>(vec.x, vec.y, vec.z);
    }
};

using BrickIndexer = BitIndexer<3>;
using SectorIndexer = BitIndexer<2>;

static_assert(BrickIndexer::to_index(-17, 0, 0) == 7);
static_assert(BrickIndexer::to_index(-16, 0, 0) == 0);
static_assert(BrickIndexer::to_index(-15, 0, 0) == 1);

static_assert(BrickIndexer::to_index(7, 0, 0) == 7);
static_assert(BrickIndexer::to_index(8, 0, 0) == 0);
static_assert(BrickIndexer::to_index(9, 0, 0) == 1);

// Brick is a 8x8x8 set of voxels
class Brick {
public:
    static constexpr std::uint16_t num_voxels = BrickIndexer::max_volume;

    Voxel &at(std::uint16_t index);
    bool is_dirty() { return dirty; }

private:
    bool dirty = false;
    std::array<Voxel, num_voxels> voxels;

    friend class VoxelMap; // will control dirty flag;
};

// Sector is a 4x4x4 set of Bricks
class Sector {
public:
    Sector();

    static constexpr std::uint8_t num_bricks = SectorIndexer::max_volume;
    Brick &at(std::uint8_t index);
    bool exists(std::uint8_t index);

private:
    static constexpr std::uint8_t invalid_index = 255; 
    std::vector<Brick> bricks;
    std::array<uint8_t, num_bricks> brick_indices;
};

struct HitResult {
    glm::ivec3 pos;
    glm::vec2 uv;
    glm::vec3 normal;
};

class VoxelMap {
public:
    VoxelMap(TerrainGenerator& generator);

    // TODO: Modify every at to [] for better performance.
    Sector &get_sector(const glm::ivec3 &pos);
    Brick &get_brick(const glm::ivec3 &pos);
    Voxel get(const glm::ivec3 &pos);

    void set(const glm::ivec3 &pos, Voxel voxel);

    void mark_brick_dirty(const glm::ivec3& brick_pos);

    std::optional<HitResult>
        raycast(glm::dvec3 origin, glm::dvec3 dir, double max_distance);

    template<std::invocable<glm::ivec3&, Brick&> Fn>
    void for_each_dirty(Fn&& fn) {
        for (auto& [pos, brick] : dirty_bricks) {
            fn(pos, brick);
        }
    }

    template<std::invocable<glm::ivec3&, Voxel&> Fn>
    void dispatch(glm::ivec3 min, glm::ivec3 max, Fn&& visit) {

        glm::ivec3 brick_min = min >> BrickIndexer::shift;
        glm::ivec3 brick_max = max >> BrickIndexer::shift;

        glm::ivec3 sector_min = brick_min >> SectorIndexer::shift;
        glm::ivec3 sector_max = brick_max >> SectorIndexer::shift;


        for (int sector_z = sector_min.z; sector_z <= sector_max.z; sector_z++) {
            for (int sector_y = sector_min.y; sector_y <= sector_max.y; sector_y++) {
                for (int sector_x = sector_min.x; sector_x <= sector_max.x; sector_x++) {
                    glm::ivec3 sector_pos = {sector_x, sector_y, sector_z};

                    glm::ivec3 sector_brick_min = sector_pos << SectorIndexer::shift;
                    glm::ivec3 sector_brick_max = sector_brick_min | SectorIndexer::mask;

                    const glm::ivec3 first_brick{
                        std::max(brick_min.x, sector_brick_min.x),
                        std::max(brick_min.y, sector_brick_min.y),
                        std::max(brick_min.z, sector_brick_min.z),
                    };

                    const glm::ivec3 last_brick{
                        std::min(brick_max.x, sector_brick_max.x),
                        std::min(brick_max.y, sector_brick_max.y),
                        std::min(brick_max.z, sector_brick_max.z),
                    };

                    Sector& sector = get_sector(sector_pos);

                    for (int brick_z = first_brick.z; brick_z <= last_brick.z; brick_z++) {
                        for (int brick_y = first_brick.y; brick_y <= last_brick.y; brick_y++) {
                            for (int brick_x = first_brick.x; brick_x <= last_brick.x; brick_x++) {

                                glm::ivec3 brick_pos = { brick_x, brick_y, brick_z };

                                glm::ivec3 voxel_min = brick_pos << BrickIndexer::shift;
                                glm::ivec3 voxel_max = voxel_min | BrickIndexer::mask;

                                glm::ivec3 first_voxel{
                                    std::max(min.x, voxel_min.x),
                                    std::max(min.y, voxel_min.y),
                                    std::max(min.z, voxel_min.z),
                                };

                                glm::ivec3 last_voxel{
                                    std::min(max.x, voxel_max.x),
                                    std::min(max.y, voxel_max.y),
                                    std::min(max.z, voxel_max.z)
                                };

                                Brick& brick = sector.at(SectorIndexer::to_index(brick_pos));;

                                for (int z = first_voxel.z; z <= last_voxel.z; z++) {
                                    for (int y = first_voxel.y; y <= last_voxel.y; y++) {
                                        for (int x = first_voxel.x; x <= last_voxel.x; x++) {
                                            std::invoke(visit, glm::ivec3(x, y, z), brick.at(BrickIndexer::to_index(x, y, z)));
                                        }
                                    }
                                }

                            }
                        }
                    }


                }
            }
        }
    }

private:
    struct DirtyBrick {
        glm::ivec3 pos;
        Brick& brick;
    };

    std::unordered_map<glm::ivec3, Sector> sectors;
    std::vector<DirtyBrick> dirty_bricks;

    TerrainGenerator& terrain_generator;

    int step_level(const glm::ivec3 &pos);
};
