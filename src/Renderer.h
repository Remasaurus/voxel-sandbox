#pragma once

#include "Camera.h"
// TODO: implement

class Renderer
{
private:
public:
    Renderer();
    ~Renderer();

    void render(const Camera &camera);
};
