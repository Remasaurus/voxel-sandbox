#include "Application.h"
#include "Clock.h"
#include "Log.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

using namespace std::chrono_literals;

Application::Application() : renderer(window) {}

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
    Clock clock;

    while (!window.should_close()) {
        clock.tick(1s / 60);
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        ImGui::EndFrame();
        ImGui::Render();

        renderer.render(camera);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.swap_buffers();
    }

    return EXIT_SUCCESS;
}
