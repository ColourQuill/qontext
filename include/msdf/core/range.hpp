#ifndef __QONTEXT_RANGE_HPP__
#define __QONTEXT_RANGE_HPP__

namespace qtxt {
    struct Range {
        double lower, upper;

        inline Range(double symmetricalWidth = 0) : lower{-0.5 * symmetricalWidth}, upper{0.5 * symmetricalWidth} {}
        inline Range(double lowerBound, double upperBound) : lower{lowerBound}, upper{upperBound} {}

        inline Range& operator*=(double factor) {
            lower *= factor;
            upper *= factor;
            return *this;
        }

        inline Range& operator/=(double divisor) {
            lower /= divisor;
            upper /= divisor;
            return *this;
        }

        inline Range operator*(double factor) const {
            return Range(lower * factor, upper * factor);
        }

        inline Range operator/(double divisor) const {
            return Range(lower / divisor, upper / divisor);
        }
    };

    inline Range operator*(double factor, const Range& range) {
        return Range(factor * range.lower, factor * range.upper);
    }

    inline Range operator+(Range a, Range b) {
        return Range(a.lower + b.lower, a.upper + b.upper);
    }
}

#endif