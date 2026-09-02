#ifndef __QONTEXT_PROJECTION_HPP__
#define __QONTEXT_PROJECTION_HPP__

// msdf
#include <msdf/core/vec2.hpp>

namespace qtxt {
    class Projection {
        public:
            Projection();
            Projection(const vec2& scale, const vec2& translate);

            vec2 project(const vec2& coord) const;
            vec2 unproject(const vec2& coord) const;
            double projectX(double x) const;
            double projectY(double y) const;
            double unprojectX(double x) const;
            double unprojectY(double y) const;
        private:
            vec2 scale;
            vec2 translate;
    };
}

#endif