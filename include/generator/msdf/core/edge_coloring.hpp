#ifndef __QONTEXT_EDGE_COLORING_HPP__
#define __QONTEXT_EDGE_COLORING_HPP__

#define MSDF_EDGE_LENGTH_PRECISION 4

namespace qtxt {
    // forward declarations
    class Shape;

    void EdgeColoringSimple(Shape& shape, double angleThreshold, unsigned long long seed = 0);
    void EdgeColoringInkTrap(Shape& shape, double angleThreshold, unsigned long long seed = 0);
    void EdgeColoringByDistance(Shape& shape, double angleThreshold, unsigned long long seed = 0);
}

#endif