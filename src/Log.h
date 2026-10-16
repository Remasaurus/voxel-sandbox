#include <print>
#include <chrono>
#include <string_view>

namespace Log {
    template<typename... Args>
    void info(std::format_string<Args...> fmt, Args&&... args) {
        auto now = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());

        std::print("[{:%T}] ", now);
        std::println(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(std::format_string<Args...> fmt, Args&&... args) {
        auto now = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
        std::print(stderr, "[{:%T}] ", now);
        std::println(stderr, fmt, std::forward<Args>(args)...);
    }
}
