#ifndef __QONTEXT_ARITHMETIC_HPP__
#define __QONTEXT_ARITHMETIC_HPP__

// std
#include <cmath>

namespace qtxt {
    template<typename T>
    inline T min(T a, T b) {
        return b < a ? b : a;
    }
    template<typename T>
    inline T max(T a, T b) {
        return a < b ? b : a;
    }
    template<typename T>
    inline T median(T a, T b, T c) {
        return max(min(a, b), min(max(a, b), c));
    }
    template<typename T, typename S>
    inline T mix(T a, T b, S weight) {
        return T((S(1) - weight) * a + weight * b);
    }
    template<typename T>
    inline T clamp(T n) {
        return n >= T(0) && n <= T(1) ? n : T(n > T(0));
    }
    template<typename T>
    inline T clamp(T n, T b) {
        return n >= T(0) && n <= b ? n : T(n > T(0)) * b;
    }
    template<typename T>
    inline T clamp(T n, T a, T b) {
        return n >= a && n <= b ? n : n < a ? a : b;
    }
    template<typename T>
    inline int sign(T n) {
        return (T(0) < n) - (n < T(0));
    }
    template<typename T>
    inline int nonZeroSign(T n) {
        return 2*(n > T(0)) - 1;
    }
}

#endif