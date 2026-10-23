#pragma once

#include <GLFW/glfw3.h>
#include <functional>
#include <unordered_map>
#include <vector>

class Window {
public:
    Window();
    ~Window();

    bool should_close() const;
    void make_current() const;
    void set_swap_interval(int interval) const;
    void swap_buffers() const;

    void on_keydown(int key, const std::function<void()> &f);
    void on_keydown(int key, std::function<void()> &&f);
    void on_keyup(int key, const std::function<void()> &f);
    void on_keyup(int key, std::function<void()> &&f);

    operator bool() const;

    GLFWwindow *get() const { return window; }

private:
    GLFWwindow *window;

    using FunctionMap =
        std::unordered_map<int, std::vector<std::function<void()>>>;
    FunctionMap on_keydowns;
    FunctionMap on_keyups;

    static void key_callback(
        GLFWwindow *window, int key, int scancode, int action, int mods
    );
};
