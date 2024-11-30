#ifndef __QONTEXT_INTERSECTION_HPP__
#define __QONTEXT_INTERSECTION_HPP__

namespace qtxt {
    // An intersection with the scanline.
    struct Intersection {
        // x coordinate
        double x;
        // Normalized y direction of the oriented edge at the point of intersection
        int direction;
    };
}

#endif