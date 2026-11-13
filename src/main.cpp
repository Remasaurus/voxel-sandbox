#include "Application.h"
#include "Log.h"
#include "VoxelMap.h"
#include <GL/glext.h>
#include <cstdlib>
#include <immintrin.h>
#include <xmmintrin.h>

namespace {
const char *gl_string(GLenum var) {
    return reinterpret_cast<const char *>(glGetString(var));
}

void log_gl() {
    Log::info("OpenGL vendor:       {}", gl_string(GL_VENDOR));
    Log::info("OpenGL renderer:     {}", gl_string(GL_RENDERER));
    Log::info("OpenGL version:      {}", gl_string(GL_VERSION));
    Log::info(
        "OpenGL glsl version: {}", gl_string(GL_SHADING_LANGUAGE_VERSION)
    );
}
} // namespace

int main(void) {
    glfwInit();

    // Testing
    VoxelMap map;
    for (int x = -64; x < 64; x++) {
        for (int y = -64; y < 64; y++) {
            for (int z = -64; z < 64; z++) {
                map.set({x, y, z}, Voxel{static_cast<uint8_t>(x ^ y ^ z)});
            }

        }
    }

    for (int x = -64; x < 64; x++) {
        for (int y = -64; y < 64; y++) {
            for (int z = -64; z < 64; z++) {
                if (map.get({x, y, z}).data != static_cast<uint8_t>(x ^ y ^ z)) {
                    Log::error("VoxelMap is not correct ({}, {}, {})", x, y, z);
                    return EXIT_FAILURE;
                }
            }
        }
    }


    auto hit = map.raycast({0, 128, 0}, {0, -1, 0}, 100);
    if (!hit) {
        Log::error("Raycast doesn't work");
        return EXIT_FAILURE;
    }

    auto pos = hit.value().pos;
    Log::info("Raycast hit: ({}, {}, {})", pos.x, pos.y, pos.z);


    Log::info("VoxelMap works as expected");

    // TODO: add command line argument support
    int exit_code;
    {
        Application app;

        log_gl();

        exit_code = app.run();
    }

    glfwTerminate();

    return exit_code;
}
