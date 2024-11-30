#ifndef __QONTEXT_SHAPE_HPP__
#define __QONTEXT_SHAPE_HPP__

#define MSDF_CORNER_DOT_EPSILON 0.000001

// std
#include <vector>

// msdf
#include <generator/msdf/core/contour.hpp>

namespace qtxt {
    // forward declarations
    struct Bounds;
    class Scanline;

    class Shape {
        public:
            std::vector<Contour> contours;

            bool inverseYAxis;

            Shape();

            void AddContour(const Contour& contour);
            Contour& AddContour();

            void Normalize();
            bool Validate() const;
            void Bound(double& l, double& b, double& r, double& t) const;
            void BoundMiters(double& l, double& b, double& r, double& t, double border, double miterLimit, int polarity) const;
            Bounds GetBounds(double border = 0, double miterLimit = 0, int polarity = 0) const;
            void DoScanline(Scanline& line, double y) const;
            int EdgeCount() const;
            void OrientContours();
        private:
    };
}

#endif