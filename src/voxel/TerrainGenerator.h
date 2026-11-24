#pragma once

#include <glm/ext/vector_int3.hpp>

class Brick;
class Sector;

class TerrainGenerator {
public:
    TerrainGenerator() = default;

    Brick get_brick(const glm::ivec3& pos);

private:
    static constexpr int ground_y = 0;
    void fill_ground(Brick *brick, const glm::ivec3& voxel_max);
};
