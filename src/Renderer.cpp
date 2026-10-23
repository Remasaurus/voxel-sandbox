#include <glad/gl.h>

#include "Log.h"
#include "Renderer.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

Renderer::Renderer(const Window &wnd) : window(wnd) {
    window.make_current();

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        Log::error("Failed to initialize opengl");
    }

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // Setup Dear ImGui style
    ImGuiStyle &style = ImGui::GetStyle();
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();
    // style.FontSizeBase = 16.0f;           // Default to 13.0f when
    // unspecified, which allows crisp legacy font for non-4K screens.
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;

    ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
    ImGui_ImplOpenGL3_Init(nullptr);

    IMGUI_CHECKVERSION();
}

Renderer::~Renderer() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Renderer::render(const Camera &camera) {
    // TODO: make this an actual render function
    (void)camera;

    glClearColor(0.1, 0.1, 0.1, 0.1);
    glClear(GL_COLOR_BUFFER_BIT);
}
