#ifndef __QONTEXT_WORKLOAD_HPP__
#define __QONTEXT_WORKLOAD_HPP__

// std
#include <functional>

namespace qtxt {
    class Workload {
        public:
            Workload();
            Workload(const std::function<bool(int, int)>& workerFunction, int chunks);

            bool finish(int threadCount);
        private:
            std::function<bool(int, int)> workerFunction;
            int chunks;

            bool finishSequential();
            bool finishParallel(int threadCount);
    };
}

#endif