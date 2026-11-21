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
    raycast_shader.set_uniform("pos", camera.position);
    raycast_shader.set_uniform("x_axis", camera.right);
    raycast_shader.set_uniform("y_axis", camera.up);
    raycast_shader.set_uniform("dir", camera.front);
    raycast_shader.set_uniform("fov", glm::radians(camera.fov));

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
