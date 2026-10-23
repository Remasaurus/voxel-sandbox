#include <chrono>

class Clock {
public:
    using duration = std::chrono::high_resolution_clock::duration;

    Clock();
    void tick(duration duration);

private:
    std::chrono::high_resolution_clock::time_point last;
};
