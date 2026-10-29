#include "Clock.h"
#include <chrono>
#include <thread>

Clock::Clock() { last = std::chrono::high_resolution_clock::now(); }

void Clock::tick(Duration time) {
    auto next = last + time;
    auto now = std::chrono::high_resolution_clock::now();
    if (now > next) {
        last = now;
        return;
    }

    std::this_thread::sleep_until(next);
    last = std::chrono::high_resolution_clock::now();
    ;
}
