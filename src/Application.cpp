#include "Application.h"
#include "Log.h"

#include <GLFW/glfw3.h>
#include <print>

Application::Application() {}

Application::~Application() {}

int Application::run() {

    // Successfully loaded OpenGL
    // TODO: refactor startup logging
    // std::println(
    //     "Loaded OpenGL %d.%d", GLAD_VERSION_MAJOR(version),
    //     GLAD_VERSION_MINOR(version)
    // );

    // std::println(
    //     "OpenGL vendor: {}",
    //     reinterpret_cast<const char *>(glGetString(GL_VENDOR))
    // );
    // std::println(
    //     "OpenGL renderer: {}",
    //     reinterpret_cast<const char *>(glGetString(GL_RENDERER))
    // );
    // std::println(
    //     "OpenGL shading version: {}",
    //     reinterpret_cast<const char
    //     *>(glGetString(GL_SHADING_LANGUAGE_VERSION))
    // );
    window.make_current();

    while (!window.should_close()) {
        window.swap_buffers();
        glfwPollEvents();
        // TODO: update controls
        // camera.update();

        // renderer.render(camera);
    }

    return EXIT_SUCCESS;
}