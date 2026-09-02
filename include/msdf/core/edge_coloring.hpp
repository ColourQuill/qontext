#ifndef __QONTEXT_EDGE_COLORING_HPP__
#define __QONTEXT_EDGE_COLORING_HPP__

// info
#include <info/properties.hpp>

namespace qtxt {
    // forward declarations
    class Shape;

    void edgeColoringSimple(Shape& shape, double angleThreshold, unsigned long long seed = 0);
    void edgeColoringInkTrap(Shape& shape, double angleThreshold, unsigned long long seed = 0);
    void edgeColoringByDistance(Shape& shape, double angleThreshold, unsigned long long seed = 0);
}

#endif