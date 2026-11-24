#pragma once

#include "Camera.h"
#include "Framebuffer.h"
#include "Shader.h"
#include "../voxel/VoxelMap.h"
#include "../Window.h"

void enable_opengl_debug();

class Renderer {
public:
    Renderer(VoxelMap& voxel_map);
    ~Renderer();

    void render(const Window& window, const Camera &camera);

    int render_distance = 100;
    int light_bounces = 2;
    bool heatmap = false;

    struct {
        int ray_casts = 0;
        int traversal_iterations = 0;
    } stats;

private:
    Shader raycast_shader;
    Framebuffer raycast_framebuffer;

    VoxelMap& voxel_map_;

    Shader blit_shader;
    unsigned empty_vao;
    Texture tex;
};
