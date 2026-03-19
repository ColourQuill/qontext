#include <utils/timer.hpp>

namespace qtxt {
    void Timer::Start() {
        start = std::chrono::high_resolution_clock::now();
    }
    double Timer::End() {
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        return duration.count();
    }
}
