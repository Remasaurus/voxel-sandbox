#include "Window.h"
#include "Log.h"
#include <GLFW/glfw3.h>

namespace {
void error_callback(int error, const char *description) {
    fprintf(stderr, "Error [%d]: %s\n", error, description);
}
} // namespace

void Window::cursor_pos_callback(GLFWwindow *window, double xpos, double ypos) {
    Window *wnd = static_cast<Window *>(glfwGetWindowUserPointer(window));
    for (auto &callback : wnd->on_mouse_moves) {
        callback(xpos - wnd->old_xpos, ypos - wnd->old_ypos);
    }
    wnd->old_xpos = xpos;
    wnd->old_ypos = ypos;
}

void Window::key_callback(
    GLFWwindow *window, int key, int scancode, int action, int mods
) {

    (void)scancode;
    (void)mods;

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

void Window::on_mouse_move(std::function<void(double, double)> &&f) {
    on_mouse_moves.push_back(std::move(f));
}

void Window::capture_mouse(bool capture) {
    if (capture) {
        if (glfwRawMouseMotionSupported()) {
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }
        glfwSetCursor(window, NULL);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        if (glfwRawMouseMotionSupported()) {
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
        }
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void Window::request_close() { glfwSetWindowShouldClose(window, GLFW_TRUE); }

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

    glfwSetKeyCallback(window, key_callback);
    glfwGetCursorPos(window, &old_xpos, &old_ypos);
    glfwSetCursorPosCallback(window, cursor_pos_callback);

    Log::info("Raw mouse motion support: {}", glfwRawMouseMotionSupported());
}

Window::~Window() { glfwDestroyWindow(window); }

bool Window::should_close() const { return glfwWindowShouldClose(window); }
void Window::make_current() const { return glfwMakeContextCurrent(window); }
void Window::set_swap_interval(int interval) const {
    return glfwSwapInterval(interval);
}
void Window::swap_buffers() const { glfwSwapBuffers(window); }

Window::operator bool() const { return window != nullptr; }
