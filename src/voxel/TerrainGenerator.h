#pragma once

#include <glm/ext/vector_int3.hpp>

class Brick;

class TerrainGenerator {
public:
    TerrainGenerator() = default;

    Brick get_brick(const glm::ivec3& pos);

private:
};
