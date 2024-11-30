#ifndef __QONTEXT_PADDING_HPP__
#define __QONTEXT_PADDING_HPP__

namespace qtxt {
    // forward declarations
    struct Bounds;

    struct Padding {
        double l, b, r, t;

        inline Padding(double uniformPadding = 0) : l(uniformPadding), b(uniformPadding), r(uniformPadding), t(uniformPadding) {}
        inline Padding(double l, double b, double r, double t) : l(l), b(b), r(r), t(t) {}
    };

    void Pad(Bounds& bounds, const Padding& padding);

    Padding operator-(const Padding& padding);
    Padding operator+(const Padding& a, const Padding& b);
    Padding operator-(const Padding& a, const Padding& b);
    Padding operator*(double factor, const Padding& padding);
    Padding operator*(const Padding& padding, double factor);
    Padding operator/(const Padding& padding, double divisor);
}

#endif