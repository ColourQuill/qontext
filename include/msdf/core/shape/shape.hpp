#ifndef __QONTEXT_SHAPE_HPP__
#define __QONTEXT_SHAPE_HPP__

#define MSDF_CORNER_DOT_EPSILON 0.000001

// std
#include <vector>

// msdf
#include <msdf/core/shape/contour.hpp>

namespace qtxt {
    // forward declarations
    struct Bounds;
    class Scanline;

    class Shape {
        public:
            std::vector<Contour> contours;

            bool inverseYAxis;

            Shape();

            void add(const Contour& contour);
            Contour& add();

            void normalize();
            bool validate() const;
            void bound(double& l, double& b, double& r, double& t) const;
            void boundMiters(double& l, double& b, double& r, double& t, double border, double miterLimit, int polarity) const;
            Bounds getBounds(double border = 0, double miterLimit = 0, int polarity = 0) const;
            void scanline(Scanline& line, double y) const;
            int edgeCount() const;
            void orient();
        private:
    };
}

#endif