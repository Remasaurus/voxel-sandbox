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

    void on_mouse_move(std::function<void(double, double)> &&f);

    void capture_mouse(bool capture = true);
    void request_close();

    operator bool() const;

    GLFWwindow *get() const { return window; }

private:
    GLFWwindow *window;

    double old_xpos = 0;
    double old_ypos = 0;

    using FunctionMap =
        std::unordered_map<int, std::vector<std::function<void()>>>;
    FunctionMap on_keydowns;
    FunctionMap on_keyups;

    std::vector<std::function<void(double, double)>> on_mouse_moves;

    static void key_callback(
        GLFWwindow *window, int key, int scancode, int action, int mods
    );
    static void cursor_pos_callback(
        GLFWwindow *window, double xpos, double ypos
    );
};
