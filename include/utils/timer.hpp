#ifndef __QONTEXT_TIMER_HPP__
#define __QONTEXT_TIMER_HPP__

#include <chrono>

namespace qtxt {
    class Timer {
        public:
            void Start();
            double End();
        private:
            std::chrono::time_point<std::chrono::high_resolution_clock> start;
            std::chrono::time_point<std::chrono::high_resolution_clock> end;
    };
}

#endif