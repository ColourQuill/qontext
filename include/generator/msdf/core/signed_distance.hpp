#ifndef __QONTEXT_SIGNED_DISTANCE_HPP__
#define __QONTEXT_SIGNED_DISTANCE_HPP__

// std
#include <cmath>
#include <cfloat>

namespace qtxt {
    class SignedDistance {
        public:
            inline SignedDistance() : distance{-DBL_MAX}, dot{0} {}
            inline SignedDistance(double distance, double dot) : distance{distance}, dot{dot} {}

            double distance;
            double dot;
    };

    inline bool operator<(const SignedDistance a, const SignedDistance b) {
        return fabs(a.distance) < fabs(b.distance) || (fabs(a.distance) == fabs(b.distance) && a.dot < b.dot);
    }

    inline bool operator>(const SignedDistance a, const SignedDistance b) {
        return fabs(a.distance) > fabs(b.distance) || (fabs(a.distance) == fabs(b.distance) && a.dot > b.dot);
    }

    inline bool operator<=(const SignedDistance a, const SignedDistance b) {
        return fabs(a.distance) < fabs(b.distance) || (fabs(a.distance) == fabs(b.distance) && a.dot <= b.dot);
    }

    inline bool operator>=(const SignedDistance a, const SignedDistance b) {
        return fabs(a.distance) > fabs(b.distance) || (fabs(a.distance) == fabs(b.distance) && a.dot >= b.dot);
    }
}

#endif