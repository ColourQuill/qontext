#ifndef __QONTEXT_RASTERIZATION_HPP__
#define __QONTEXT_RASTERIZATION_HPP__

// std
#include <vector>

// msdf
#include <msdf/core/scanline.hpp>
#include <msdf/core/image.hpp>

namespace qtxt {
    // forward declarations
    class Projection;
    class Shape;

    // Rasterizes the shape into a monochrome bitmap
    void rasterize(Image<float, 1>& output, const Shape& shape, const Projection& projection, FillRule fillRule = FillRule::NON_ZERO);
    // Fixes the sign of the input signed distance field, so that it matches the shape's rasterized fill
    void distanceSignCorrection(Image<float, 1>& sdf, const Shape& shape, const Projection& projection, FillRule fillRule = FillRule::NON_ZERO);
    void distanceSignCorrection(Image<float, 3>& sdf, const Shape& shape, const Projection& projection, FillRule fillRule = FillRule::NON_ZERO);
    void distanceSignCorrection(Image<float, 4>& sdf, const Shape& shape, const Projection& projection, FillRule fillRule = FillRule::NON_ZERO);
}

#endif