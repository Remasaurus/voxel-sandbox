#pragma once

#include "Brush.h"
#include "Window.h"
#include "render/Camera.h"
#include "render/Renderer.h"
#include "voxel/VoxelMap.h"

#include <chrono>
class Application {
    using duration = std::chrono::high_resolution_clock::duration;

public:
    Application();
    ~Application();

    int run();
    static constexpr int tick_rate = 60;

private:
    float movement_speed = 0.5;
    float rotation_speed = 0.5;

    bool show_settings = false;

    Camera camera;
    Window window;
    Renderer renderer;
    VoxelMap voxel_map;
    Brush brush;

    void setup_imgui();
    void render_options_menu(
        duration waiting, duration updating,
        duration imgui_render_time, duration render_time
    );
};
