#pragma once

#include <GLFW/glfw3.h>

class Window {
private:
  GLFWwindow *window;

public:
  Window();
  ~Window();

  bool should_close() const;
  void make_current() const { return glfwMakeContextCurrent(window); }
  void set_swap_interval(int interval) const;

  operator bool() const;
};