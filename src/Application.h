#pragma once

#include "Camera.h"
#include "Renderer.h"
#include "Window.h"

class Application {
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
};
