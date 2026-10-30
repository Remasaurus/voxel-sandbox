#pragma once

#include "Camera.h"

void enable_opengl_debug();

class Renderer {
public:
    Renderer();
    ~Renderer();

    void render(const Camera &camera) const;
    int render_distance = 100;
    int light_bounces = 2;
    bool heatmap = false;

    struct Stats {
        int ray_casts = 0;
        int traversal_iterations = 0;
    };
    Stats stats() const;

private:
    Stats stats_;
};
