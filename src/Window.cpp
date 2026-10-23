#include "Window.h"
#include "Log.h"

namespace {
void error_callback(int error, const char *description) {
    fprintf(stderr, "Error [%d]: %s\n", error, description);
}
} // namespace

void Window::key_callback(
    GLFWwindow *window, int key, int scancode, int action, int mods
) {

    (void)scancode;
    (void)mods;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    Window *wnd = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (!wnd)
        return;

    Window::FunctionMap *map;
    if (action == GLFW_PRESS) {
        map = &wnd->on_keydowns;
    } else if (action == GLFW_RELEASE) {
        map = &wnd->on_keyups;
    } else {
        return;
    }

    auto result = map->find(key);
    if (result == map->end())
        return;

    for (const auto &callback : result->second) {
        callback();
    }
}

void Window::on_keydown(int key, const std::function<void()> &f) {
    on_keydowns[key].push_back(f);
}
void Window::on_keydown(int key, std::function<void()> &&f) {
    on_keydowns[key].push_back(std::move(f));
}
void Window::on_keyup(int key, const std::function<void()> &f) {
    on_keyups[key].push_back(f);
}
void Window::on_keyup(int key, std::function<void()> &&f) {
    on_keyups[key].push_back(std::move(f));
}

Window::Window() {

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    glfwSetErrorCallback(error_callback);

    window = glfwCreateWindow(640, 480, "Voxel Sandbox", NULL, NULL);
    if (!window) {
        Log::error("Unable to create GLFW window");
        return;
    }

    glfwSetWindowUserPointer(window, this);

    window = glfwCreateWindow(640, 480, "Voxel Sandbox", NULL, NULL);
    if (!window) {
        Log::error("Unable to create GLFW window");
        return;
    }

    glfwSetKeyCallback(window, key_callback);
}

Window::~Window() { glfwDestroyWindow(window); }

bool Window::should_close() const { return glfwWindowShouldClose(window); }
void Window::make_current() const { return glfwMakeContextCurrent(window); }
void Window::set_swap_interval(int interval) const {
    return glfwSwapInterval(interval);
}
void Window::swap_buffers() const { glfwSwapBuffers(window); }

Window::operator bool() const { return window != nullptr; }
