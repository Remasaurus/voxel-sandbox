#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL // Needed for hashing glm::ivec3
#include <glm/gtx/hash.hpp>
#include <optional>
#include <vector>

struct Voxel {
    std::uint8_t data;
};

template<size_t Shift>
struct BitIndexer {

    static constexpr int shift = Shift;
    static constexpr int mask = (1 << shift) - 1;
    static constexpr int max_length = 1 << shift;
    static constexpr int max_area = max_length * max_length;
    static constexpr int max_volume = max_area * max_length;

    template<typename T>
    static constexpr T to_index(T x, T y, T z) {
        // x + y * max_length + z * max_area but with correct bitshifting
        return (x & mask) | ((y & mask) << shift) | ((z & mask) << shift * 2);
    }


    template<typename T>
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

    Voxel& at(std::uint16_t index);

private:
    std::array<Voxel, num_voxels> voxels;
};

// Sector is a 4x4x4 set of Bricks
class Sector {
public:
    Sector();

    static constexpr std::uint8_t num_bricks = SectorIndexer::max_volume;
    Brick& at(std::uint8_t index);

private:
    static constexpr std::uint8_t invalid_index = 255; // This is never used: 4 * 4 * 4 = 64 < 255
    std::vector<Brick> bricks;
    std::array<uint8_t, num_bricks> brick_indices;
};


struct HitResult {
    glm::vec3 normal;
    glm::vec2 uv;
    glm::ivec3 pos;
};

class VoxelMap {
public:

    // TODO: Modify every at to [] for better performance.
    Sector& get_sector(const glm::ivec3& pos);
    Brick& get_brick(const glm::ivec3& pos);
    Voxel get(const glm::ivec3& pos);

    void set(const glm::ivec3& pos, Voxel voxel);

    std::optional<HitResult> raycast(glm::dvec3 origin, glm::dvec3 dir, double max_distance);

private:
    std::unordered_map<glm::ivec3, Sector> sectors;
};
