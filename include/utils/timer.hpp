#ifndef __QONTEXT_TIMER_HPP__
#define __QONTEXT_TIMER_HPP__

#include <chrono>

namespace qtxt {
    class Timer {
        public:
            void start();
            double end();
        private:
            std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
            std::chrono::time_point<std::chrono::high_resolution_clock> m_end;
    };
}

#endif