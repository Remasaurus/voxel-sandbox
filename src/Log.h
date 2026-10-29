#include <chrono>
#include <print>
#include <string_view>

namespace Log {
template <typename... Args>
void info(std::format_string<Args...> fmt, Args &&...args) {
    auto now =
        std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
    std::print("[{}] ", now);
    std::println(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void error(std::format_string<Args...> fmt, Args &&...args) {
    auto now =
        std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
    std::print(stderr, "[{}] ", now);
    std::println(stderr, fmt, std::forward<Args>(args)...);
}
} // namespace Log
