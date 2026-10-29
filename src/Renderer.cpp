#include <glad/gl.h>
#include <imgui.h>

#include "Log.h"
#include "Renderer.h"

Renderer::Renderer() {}

Renderer::~Renderer() {}

void Renderer::render(const Camera &camera) const {
    // TODO: make this an actual render function
    (void)camera;

    glClearColor(1, 0.4, 0.8, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

Renderer::Stats Renderer::stats() const { return stats_; }
