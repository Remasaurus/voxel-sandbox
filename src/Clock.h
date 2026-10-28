#include <chrono>

class Clock {
public:
    using Duration = std::chrono::high_resolution_clock::duration;

    Clock();
    void tick(Duration duration);

private:
    std::chrono::high_resolution_clock::time_point last;
};
