#include <glad/gl.h>
#include <imgui.h>

#include "../Log.h"
#include "Renderer.h"

unsigned texture1;
Renderer::Renderer() {
    Log::info("Initializing shaders");

    blit_shader =
        create_pixel_shader("resources/blit.vert", "resources/blit.frag");
    glCreateVertexArrays(1, &empty_vao);

    raycast_shader = create_compute_shader("resources/raycast.comp");

    std::uint8_t color[3] = { 255, 255, 255 };

    tex = Texture(1, 1, GL_RGB8);
    tex.set_parameters({
        { GL_TEXTURE_WRAP_S, GL_REPEAT },
        { GL_TEXTURE_WRAP_T, GL_REPEAT },
        { GL_TEXTURE_MIN_FILTER, GL_LINEAR },
        { GL_TEXTURE_MAG_FILTER, GL_LINEAR },
    });
    tex.write(GL_RGB, color);

    Log::info("Renderer has been setup");
}

Renderer::~Renderer() {}


void Renderer::render(const Window& window, const Camera &camera) {
    if (raycast_framebuffer.size() != camera.size) {
        Log::info("Resizing framebuffer to {} x {}", camera.size.x, camera.size.y);
        raycast_framebuffer.resize(camera.size.x, camera.size.y);
    }

    glClearColor(1, 0.4, 0.8, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // Raycast voxel
    raycast_shader.use();
    raycast_framebuffer.as_texture().bind_image_texture(0, GL_WRITE_ONLY, GL_RGBA32F);
    glDispatchCompute(camera.size.x, camera.size.y, 1);


    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // Copy voxel to screen
    blit_shader.use();

    auto size = window.size();
    glViewport(0, 0, size.x, size.y);
    raycast_framebuffer.as_texture().bind_texture_unit(0);
    blit_shader.set_uniform("scene", 0);
    glBindVertexArray(empty_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
