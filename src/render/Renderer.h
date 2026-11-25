#pragma once

#include "Camera.h"
#include "Framebuffer.h"
#include "Shader.h"
#include "Buffer.h"
#include "../voxel/VoxelMap.h"
#include "../Window.h"

void enable_opengl_debug();

class Renderer {
public:
    Renderer(VoxelMap& voxel_map);
    ~Renderer();

    void render(const Window& window, const Camera &camera);

    unsigned light_bounces = 2;
    bool heatmap = false;

    static constexpr unsigned max_requests_per_frame = 1024;

    struct {
        int ray_casts = 0;
        int traversal_iterations = 0;
    } stats;

private:
    void process_request_buffer();
    void allocate_brick_grid(unsigned new_render_distance);


    unsigned render_distance = 0;

    Shader raycast_shader;
    Framebuffer raycast_framebuffer;

    VoxelMap& voxel_map_;

    Shader blit_shader;
    unsigned empty_vao;

    Shader upload_shader;

    struct Request {
        glm::ivec3 pos;
    };

    struct BrickMapElement {
        std::uint32_t solid_mask[16];
        std::uint32_t voxel_offset;
    };


    Buffer chunk_grid;
    Buffer brick_grid;
    Buffer brick_map;
    Buffer voxel_data;
    Buffer request_buffer;
    std::vector<Request> request_buffer_data;

    Buffer upload_buffer;
    std::vector<BrickMapElement> upload_buffer_data;
};
