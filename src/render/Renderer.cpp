#include <glad/gl.h>
#include <imgui.h>

#include "Renderer.h"
#include "../Log.h"

unsigned texture1;
Renderer::Renderer(VoxelMap& voxel_map): voxel_map_(voxel_map) {
    Log::info("Initializing shaders");

    blit_shader =
        create_pixel_shader("resources/blit.vert", "resources/blit.frag");
    glCreateVertexArrays(1, &empty_vao);

    raycast_shader = create_compute_shader("resources/raycast.comp");

    unsigned full_render_length = 2 * render_distance;
    unsigned brick_grid_cell_count = full_render_length * full_render_length * full_render_length;
    unsigned brick_grid_byte_count = brick_grid_cell_count * 4;
    brick_grid = Buffer(brick_grid_byte_count);

    std::vector<std::uint32_t> grid_pointers(brick_grid_cell_count, 128);
    brick_grid.write(0, brick_grid_byte_count, grid_pointers.data());
    brick_grid.bind_buffer_base(1);

    Log::info("Brick grid is {} bytes ({} MB)", brick_grid_byte_count, brick_grid_byte_count / 1000000);

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
    raycast_shader.set_uniform("focal_length", 1 / std::tanf(0.5 * glm::radians(camera.fov)));

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
