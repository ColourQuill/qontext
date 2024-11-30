#ifndef __QONTEXT_TIMER_HPP__
#define __QONTEXT_TIMER_HPP__

#include <chrono>

namespace qtxt {
    class Timer {
        public:
            void start() {
                start_v = std::chrono::high_resolution_clock::now();
            }
            double end() {
                end_v = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = end_v - start_v;
                return duration.count();
            }
        private:
            std::chrono::time_point<std::chrono::high_resolution_clock> start_v;
            std::chrono::time_point<std::chrono::high_resolution_clock> end_v;
    };
}

#endif