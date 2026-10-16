#include "Window.h"
#include "Log.h"

#include <glad/gl.h>

static void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

Window::Window() {

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

  window = glfwCreateWindow(640, 480, "Voxel Sandbox", NULL, NULL);
  if (!window) {
    Log::error("Unable to create GLFW window");
    return;
  }

}

Window::~Window() { glfwDestroyWindow(window); }

bool Window::should_close() const { return glfwWindowShouldClose(window); }
void Window::make_current() const { return glfwMakeContextCurrent(window); }
void Window::set_swap_interval(int interval) const {
  return glfwSwapInterval(interval);
}

Window::operator bool() const { return window != nullptr; }
