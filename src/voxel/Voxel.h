#pragma once

#include <cstdint>
#include <glm/ext/vector_uint3_sized.hpp>


struct Color {
    constexpr Color(uint32_t rgba)
        : red(rgba & 0xff), green((rgba << 8) & 0xff),
          blue((rgba << 16) & 0xff), alpha((rgba << 24) & 0xff) {}

    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        : red(r), green(g), blue(b), alpha(a) {}

    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
};

enum class Material : std::uint8_t {
    Air,

    // Solids
    Wood,
    Leaf,

    // Powders
    Sand,

    // Fluids
    Water,

    // Gas
    Steam,
};

struct Voxel {
    std::uint8_t data;

    bool isAir() { return data == 0; }
};

