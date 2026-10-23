#pragma once

#include "Camera.h"
#include "Window.h"
#include "Renderer.h"

class Application {
private:
  Camera camera;
  Window window;
  Renderer renderer;

public:
  Application();
  ~Application();

  int run();
};