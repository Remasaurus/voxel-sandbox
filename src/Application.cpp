#include "Application.h"
#include "Clock.h"
#include "Log.h"
#include <chrono>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

using namespace std::chrono_literals;

Application::Application(): voxel_map(terrain_generator), renderer(voxel_map) {

    setup_imgui();

    window.on_keydown(GLFW_KEY_ESCAPE, [this]() {
        show_settings = !show_settings;
        if (show_settings) {
            ImGui::SetNextWindowSize(ImVec2(0.01f, 0.01f));
        }
        window.capture_mouse(!show_settings);
    });

    window.capture_mouse();

    window.on_mouse_move([this](double dx, double dy) {
        if (!show_settings) {
            camera.rotate(glm::vec2(dx, -dy) * (rotation_speed * 0.01f));
        }
    });

    window.on_resize([&](int width, int height) {
        Log::info("Resizing to {} x {}", width, height);
        camera.size = {
            width / pixels_per_ray,
            height / pixels_per_ray,
        };
    });

    // Some window managers dont start window with resize event
    glm::vec2 size = window.size();
    camera.size = glm::uvec2(size / pixels_per_ray);
}

Application::~Application() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Application::setup_imgui() {
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.Fonts->AddFontDefaultVector();
    io.IniFilename = NULL;
    io.LogFilename = NULL;

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

    enable_opengl_debug();

    IMGUI_CHECKVERSION();
}

void Application::render_options_menu(
    duration waiting, duration updating, duration imgui_render_time,
    duration render_time
) {
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
            ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always,
            ImVec2(0.5f, 0.5f)
        );
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
        ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoDecoration);

        ImGui::SeparatorText("General");
        ImGui::SliderFloat("Movement speed", &movement_speed, 0, 1);
        ImGui::SliderFloat("Rotation speed", &rotation_speed, 0, 1);
        ImGui::SliderFloat("FOV", &camera.fov, 10, 120);
        ImGui::SliderFloat("Pixels per ray", &pixels_per_ray, 1, 4);
        // ImGui::SliderScalar("Light Bounces", &renderer.light_bounces, 0u, 5u);
        unsigned min_render_distance = 50, max_render_distance = 300;
        ImGui::SliderScalar("Render distance", ImGuiDataType_U32, &renderer.render_distance, &min_render_distance, &max_render_distance);

        ImGui::Checkbox("Heatmap", &renderer.heatmap);

        if (ImGui::Button("Quit Game")) {
            window.request_close();
        }
        if (ImGui::Button("Close")) {
            show_settings = false;
            window.capture_mouse(true);
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
        std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(
            waiting
        )
            .count()
    );
    ImGui::Text(
        "Updating: %.2fms",
        std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(
            updating
        )
            .count()
    );
    ImGui::Text(
        "ImGUI: %.2fms",
        std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(
            imgui_render_time
        )
            .count()
    );
    ImGui::Text(
        "Voxel Rendering: %.2fms",
        std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(
            render_time
        )
            .count()
    );

    ImGui::SeparatorText("Renderer");
    auto stats = renderer.stats;
    ImGui::Text("Render: %.2fms (%.2fmrays/s)", 0.f, 0.f);
    ImGui::Text("Traversal: %.2f iter/ray", 0.f);

    ImGui::End();
    ImGui::EndFrame();
    ImGui::Render();
}

void Application::update_voxels() {
    voxel_map.for_each_dirty([](const glm::ivec3& pos, Brick& brick) {
        (void)pos;
        (void)brick;
    });
}

int Application::run() {

    Clock clock;
    auto frametime = std::chrono::nanoseconds(1s) / tick_rate;

    Log::info(
        "Target tick rate {} ({} ms)", tick_rate,
        frametime.count() / 1'000'000.0
    );

    // time-keeping variables
    auto before = std::chrono::high_resolution_clock::now();
    duration waiting{};
    duration updating{};
    duration imgui_render_time{};
    duration render_time{};

    while (!window.should_close()) {
        // forced frame time delay
        // TODO: dynamic framerate with fixed game physics rate
        before = std::chrono::high_resolution_clock::now();
        clock.tick(frametime);
        waiting = std::chrono::high_resolution_clock::now() - before;

        // game update logic
        before = std::chrono::high_resolution_clock::now();
        glfwPollEvents();
        camera.move(window.move_input(), 1.0 / 60.0); //TODO: dynamic deltatime
        // TODO: implement actual game update logic
        updating = std::chrono::high_resolution_clock::now() - before;

        // scene rendering
        before = std::chrono::high_resolution_clock::now();
        renderer.render(window, camera);
        render_time = std::chrono::high_resolution_clock::now() - before;

        // GUI rendering
        before = std::chrono::high_resolution_clock::now();
        render_options_menu(waiting, updating, render_time, imgui_render_time);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        imgui_render_time = std::chrono::high_resolution_clock::now() - before;

        window.swap_buffers();
    }

    return EXIT_SUCCESS;
}
