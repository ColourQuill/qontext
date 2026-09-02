#ifndef __POINT_BOUNDS_HPP__
#define __POINT_BOUNDS_HPP__

// msdf
#include <msdf/core/vec2.hpp>

namespace qtxt {
    static void pointBounds(vec2 p, double& l, double& b, double& r, double& t) {
        if (p.x < l) l = p.x;
        if (p.y < b) b = p.y;
        if (p.x > r) r = p.x;
        if (p.y > t) t = p.y;
    }
}

#endif