#include "Clock.h"
#include <chrono>
#include <thread>

Clock::Clock() { last = std::chrono::high_resolution_clock::now(); }

void Clock::tick(duration time) {
    auto next = last + time;
    auto now = std::chrono::high_resolution_clock::now();
    if (next > now)
        return;

    std::this_thread::sleep_until(next);
}
