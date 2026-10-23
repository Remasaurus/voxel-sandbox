#pragma once

#include <GLFW/glfw3.h>

class Window {
public:
    Window();
    ~Window();

    bool should_close() const;
    void make_current() const;
    void set_swap_interval(int interval) const;
    void swap_buffers() const;

    operator bool() const;

private:
    GLFWwindow *window;
};