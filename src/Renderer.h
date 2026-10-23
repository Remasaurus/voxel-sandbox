#pragma once

#include "Camera.h"
#include "Window.h"
// TODO: implement

class Renderer {
private:
    const Window &window;

public:
    Renderer(const Window &window);
    ~Renderer();

    void render(const Camera &camera);
};
