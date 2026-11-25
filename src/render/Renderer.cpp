#include <glad/gl.h>
#include <imgui.h>
#include <iterator>

#include "Renderer.h"
#include "../Log.h"

Renderer::Renderer(VoxelMap& voxel_map): voxel_map_(voxel_map) {
    Log::info("Initializing shaders");
    glCreateVertexArrays(1, &empty_vao);

    blit_shader =
        create_pixel_shader("resources/blit.vert", "resources/blit.frag");

    raycast_shader = create_compute_shader("resources/raycast.comp");
    upload_shader = create_compute_shader("resources/upload.comp");

    // Initialize brick_grid
    unsigned full_render_length = 2 * render_distance;
    unsigned brick_grid_cell_count = full_render_length * full_render_length * full_render_length;
    unsigned brick_grid_byte_count = brick_grid_cell_count * 4;
    brick_grid = Buffer(brick_grid_byte_count);

    std::vector<std::uint32_t> grid_pointers(brick_grid_cell_count, 1);
    brick_grid.write(grid_pointers);
    brick_grid.bind_buffer_base(1);
    Log::info("Brick grid is {} bytes ({} MiB)", brick_grid_byte_count, brick_grid_byte_count / (1024 * 1024));

    // Initialize request_buffer
    unsigned request_buffer_byte_count = max_requests_per_frame * sizeof(Request) + 4; // 4 is for count
    request_buffer = Buffer(request_buffer_byte_count);
    request_buffer.bind_buffer_base(4);
    request_buffer_data.reserve(max_requests_per_frame);
    std::uint32_t zero = 0;
    request_buffer.write(0, sizeof(zero), &zero);

    // Initialize upload_buffer
    unsigned upload_buffer_byte_count = max_requests_per_frame * sizeof(BrickMapElement) + 4;
    upload_buffer = Buffer(upload_buffer_byte_count);
    upload_buffer.bind_buffer_base(5);
    upload_buffer_data.reserve(max_requests_per_frame);

    Log::info("Renderer has been setup");
}

Renderer::~Renderer() {}

void Renderer::allocate_brick_grid(unsigned new_render_distance) {
    render_distance = new_render_distance;
}

void Renderer::process_request_buffer() {
    std::uint32_t request_count;
    request_buffer.read(0, sizeof(request_count), &request_count);
    request_count = std::min(request_count, max_requests_per_frame);

    if (request_count == 0) {
        return;
    }

    request_buffer_data.resize(request_count);
    request_buffer.read(0, request_count * sizeof(Request), request_buffer_data.data());

    upload_buffer_data.resize(request_count);
    for (unsigned i = 0; i < request_count; i++) {
        Brick& brick = voxel_map_.get_brick(request_buffer_data[i].pos);
        create_solid_mask(brick, upload_buffer_data[i].solid_mask);
        upload_buffer_data[i].voxel_offset = 0;
    }
    upload_buffer.write(0, request_count * sizeof(BrickMapElement), upload_buffer_data.data());

    std::uint32_t zero = 0;
    request_buffer.write(0, sizeof(zero), &zero);

    // upload_shader.use();
    // glDispatchCompute(request_count, 1, 1);
}

void Renderer::render(const Window& window, const Camera &camera) {
    if (raycast_framebuffer.size() != camera.size) {
        Log::info("Resizing framebuffer to {} x {}", camera.size.x, camera.size.y);
        raycast_framebuffer.resize(camera.size.x, camera.size.y);
    }

    if (camera.render_distance != render_distance) {
        allocate_brick_grid(camera.render_distance);
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
    raycast_shader.set_uniform("render_distance", unsigned(render_distance * 8));

    raycast_framebuffer.as_texture().bind_image_texture(0, GL_WRITE_ONLY, GL_RGBA32F);
    glDispatchCompute(camera.size.x, camera.size.y, 1);

    // glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    process_request_buffer();

    // Copy voxel to screen
    blit_shader.use();

    auto size = window.size();
    glViewport(0, 0, size.x, size.y);
    raycast_framebuffer.as_texture().bind_texture_unit(0);
    blit_shader.set_uniform("scene", int(0));
    glBindVertexArray(empty_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
