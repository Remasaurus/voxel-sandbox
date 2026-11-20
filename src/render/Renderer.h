#pragma once

#include "Camera.h"
#include "Framebuffer.h"
#include "Shader.h"

void enable_opengl_debug();

class Renderer {
public:
    Renderer();
    ~Renderer();

    void render(const Camera &camera);

    int render_distance = 100;
    int light_bounces = 2;
    bool heatmap = false;

    struct {
        int ray_casts = 0;
        int traversal_iterations = 0;
    } stats;

private:
    glm::uvec2 size;

    Shader raycast_shader;
    Framebuffer raycast_framebuffer;

    Shader blit_shader;
    unsigned empty_vao;
};
