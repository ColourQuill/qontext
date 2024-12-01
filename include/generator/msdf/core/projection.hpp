#ifndef __QONTEXT_PROJECTION_HPP__
#define __QONTEXT_PROJECTION_HPP__

// qarbon
#include <qarbon.hpp>

namespace qtxt {
    using vec2 = qbn::vec2<double>;

    class Projection {
        public:
            Projection();
            Projection(const vec2& scale, const vec2& translate);

            // Converts the shape coordinate to pixel coordinate
            vec2 Project(const vec2& coord) const;
            // Converts the pixel coordinate to shape coordinate
            vec2 Unproject(const vec2& coord) const;
            // Converts the X-coordinate from shape to pixel coordinate space
            double ProjectX(double x) const;
            // Converts the Y-coordinate from shape to pixel coordinate space
            double ProjectY(double y) const;
            // Converts the X-coordinate from pixel to shape coordinate space
            double UnprojectX(double x) const;
            // Converts the Y-coordinate from pixel to shape coordinate space
            double UnprojectY(double y) const;
        private:
            vec2 scale;
            vec2 translate;
    };
}

#endif