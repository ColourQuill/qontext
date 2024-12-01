#ifndef __QONTEXT_SDF_ERROR_ESTIMATION_HPP__
#define __QONTEXT_SDF_ERROR_ESTIMATION_HPP__

// std
#include <vector>

// qarbon
#include <qarbon.hpp>

// msdf
#include <generator/msdf/core/scanline.hpp>
#include <generator/msdf/core/image_ref.hpp>

namespace qtxt {
    using vec2 = qbn::vec2<double>;

    // forward declarations
    class Projection;
    class Shape;
    
    // Analytically constructs a scanline at y evaluating fill by linear interpolation of the SDF
    void ScanlineSDF(Scanline& line, const ImageConstRef<float, 1>& sdf, const Projection& projection, double y, bool inverseYAxis = false);
    void ScanlineSDF(Scanline& line, const ImageConstRef<float, 3>& sdf, const Projection& projection, double y, bool inverseYAxis = false);
    void ScanlineSDF(Scanline& line, const ImageConstRef<float, 4>& sdf, const Projection& projection, double y, bool inverseYAxis = false);

    // Estimates the portion of the area that will be filled incorrectly when rendering using the SDF
    double EstimateSDFError(const ImageConstRef<float, 1>& sdf, const Shape& shape, const Projection& projection, int scanlinesPerRow, FillRule fillRule = FILL_NONZERO);
    double EstimateSDFError(const ImageConstRef<float, 3>& sdf, const Shape& shape, const Projection& projection, int scanlinesPerRow, FillRule fillRule = FILL_NONZERO);
    double EstimateSDFError(const ImageConstRef<float, 4>& sdf, const Shape& shape, const Projection& projection, int scanlinesPerRow, FillRule fillRule = FILL_NONZERO);
}

#endif