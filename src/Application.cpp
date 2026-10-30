#include <glad/gl.h>

#include "Application.h"
#include "Clock.h"
#include "Log.h"
#include <chrono>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

using namespace std::chrono_literals;

Application::Application() {
    window.make_current();

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        Log::error("Failed to initialize opengl");
    }

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.Fonts->AddFontDefaultVector();

    // Setup Dear ImGui style
    ImGuiStyle &style = ImGui::GetStyle();
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();
    style.FontSizeBase = 16.0f; // Default to 13.0f when unspecified, which
                                // allows crisp legacy font for non-4K screens.
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;

    ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
    ImGui_ImplOpenGL3_Init(nullptr);

    IMGUI_CHECKVERSION();
    window.on_keydown('W', []() { Log::info("Moving forward"); });

    window.on_keydown(GLFW_KEY_ESCAPE, [this]() {
        show_settings = !show_settings;
        window.capture_mouse(!show_settings);
    });

    window.capture_mouse();

    window.on_mouse_move([](double dx, double dy) {
        Log::info("Mouse moved ({}, {})", dx, dy);
    });
}

Application::~Application() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

int Application::run() {

    Clock clock;
    auto frametime = std::chrono::nanoseconds(1s) / tick_rate;

    Log::info(
        "Target tick rate {} ({} ms)", tick_rate,
        frametime.count() / 1'000'000.0
    );

    auto before = std::chrono::high_resolution_clock::now();
    auto after = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::duration imgui_render_time{};
    std::chrono::high_resolution_clock::duration render_time{};

    while (!window.should_close()) {
        before = std::chrono::high_resolution_clock::now();
        clock.tick(frametime);
        after = std::chrono::high_resolution_clock::now();
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        // auto [xscale, yscale] = window.scale();
        // ImGuiStyle &style = ImGui::GetStyle();
        // if (xscale != yscale) {
        // 	Log::error("window xscale and yscale are not equal! [{} != {}]",
        // xscale, yscale);
        // }
        // style.FontScaleDpi = xscale;

        ImGui::NewFrame();
        if (show_settings) {
            ImGui::SetNextWindowPos(
                ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Once,
                ImVec2(0.5f, 0.5f)
            );
            ImGui::Begin("Settings");

            ImGui::SeparatorText("General");
            ImGui::SliderFloat("Movement speed", &movement_speed, 0, 1);
            ImGui::SliderFloat("Rotation speed", &rotation_speed, 0, 1);
            ImGui::SliderFloat("FOV", &camera.fov, 10, 120);
            ImGui::SliderInt("Light Bounces", &renderer.light_bounces, 0, 5);
            ImGui::SliderInt(
                "Render distance", &renderer.render_distance, 10, 200
            );
            ImGui::Checkbox("Heatmap", &renderer.heatmap);

            if (ImGui::Button("Close")) {
                window.request_close();
            }
            ImGui::End();
        }

        ImGui::SetNextWindowPos({0, 0});
        ImGui::Begin(
            "Stats", nullptr,
            ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration
        );
        ImGui::SeparatorText("Stats");
        float framerate = ImGui::GetIO().Framerate;
        float runtime = ImGui::GetTime();
        ImGui::Text("Runtime: %.2fs", runtime);
        ImGui::Text("Frame: %.2fms (%.1f FPS)", 1000.0 / framerate, framerate);
        ImGui::Text(
            "Waiting: %.2fms",
            std::chrono::duration_cast<
                std::chrono::duration<double, std::milli>>(after - before)
                .count()
        );
        ImGui::Text(
            "ImGUI: %.2fms",
            std::chrono::duration_cast<
                std::chrono::duration<double, std::milli>>(imgui_render_time)
                .count()
        );
        ImGui::Text(
            "Voxel Rendering: %.2fms",
            std::chrono::duration_cast<
                std::chrono::duration<double, std::milli>>(render_time)
                .count()
        );

        ImGui::SeparatorText("Renderer");
        auto stats = renderer.stats();
        ImGui::Text("Render: %.2fms (%.2fmrays/s)", 0.f, 0.f);
        ImGui::Text("Traversal: %.2f iter/ray", 0.f);

        ImGui::End();
        ImGui::EndFrame();
        ImGui::Render();

        before = std::chrono::high_resolution_clock::now();
        renderer.render(camera);
        after = std::chrono::high_resolution_clock::now();
        render_time = after - before;

        before = std::chrono::high_resolution_clock::now();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        after = std::chrono::high_resolution_clock::now();
        imgui_render_time = after - before;

        window.swap_buffers();
    }

    return EXIT_SUCCESS;
}
