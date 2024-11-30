#ifndef __QONTEXT_RASTERIZATON_HPP__
#define __QONTEXT_RASTERIZATION_HPP__

// qarbon
#include <qarbon.hpp>

// msdf
#include <generator/msdf/core/scanline.hpp>
#include <generator/msdf/core/image_ref.hpp>

namespace qtxt {
    // forward declarations
    class Projection;
    class Shape;

    // Rasterizes the shape into a monochrome bitmap
    void Rasterize(const ImageRef<float, 1>& output, const Shape& shape, const Projection& projection, FillRule fillRule = FILL_NONZERO);
    // Fixes the sign of the input signed distance field, so that it matches the shape's rasterized fill
    void DistanceSignCorrection(const ImageRef<float, 1>& sdf, const Shape& shape, const Projection& projection, FillRule fillRule = FILL_NONZERO);
    void DistanceSignCorrection(const ImageRef<float, 3>& sdf, const Shape& shape, const Projection& projection, FillRule fillRule = FILL_NONZERO);
    void DistanceSignCorrection(const ImageRef<float, 4>& sdf, const Shape& shape, const Projection& projection, FillRule fillRule = FILL_NONZERO);
}

#endif