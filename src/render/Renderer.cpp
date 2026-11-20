#include <glad/gl.h>
#include <imgui.h>

#include "../Log.h"
#include "Renderer.h"

Renderer::Renderer() {
    Log::info("Initializing shaders");

    blit_shader =
        create_pixel_shader("resources/blit.vert", "resources/blit.frag");
    glCreateVertexArrays(1, &empty_vao);

    raycast_shader = create_compute_shader("resources/raycast.comp");

    Log::info("Renderer has been setup");
}

Renderer::~Renderer() {}

void Renderer::render(const Camera &camera) {
    // TODO: make this an actual render function
    (void)camera;
    if (size != camera.size) {
        size = camera.size;
        // raycast_framebuffer.resize(size.x, size.y);
    }

    glClearColor(1, 0.4, 0.8, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // raycast_shader.use();
    // raycast_framebuffer.use();
    // glDispatchCompute(size.x, size.y, 1);

    blit_shader.use();

    glBindVertexArray(empty_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
