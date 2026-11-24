#pragma once

#include <glm/ext/vector_int3.hpp>

class Brick;
class Sector;

class TerrainGenerator {
public:
    TerrainGenerator() = default;

    Brick get_brick(const glm::ivec3& pos);
    Sector get_sector(const glm::ivec3& pos);

private:
};
