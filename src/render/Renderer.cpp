#include <glad/gl.h>
#include <imgui.h>

#include "../Log.h"
#include "Renderer.h"

Renderer::Renderer() {
    Log::info("Initializing shaders");

    blit_shader = create_pixel_shader(
        "resources/blit.vert",
        "resources/blit.frag"
    );
    glCreateVertexArrays(1, &empty_vao);

    Log::info("Renderer has been setup");
}

Renderer::~Renderer() {}

void Renderer::render(const Camera &camera) {
    // TODO: make this an actual render function
    (void)camera;

    glClearColor(1, 0.4, 0.8, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    blit_shader.use();
    glBindVertexArray(empty_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
