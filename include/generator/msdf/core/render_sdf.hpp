#ifndef __QONTEXT_RENDER_SDF_HPP__
#define __QONTEXT_RENDER_SDF_HPP__

// qarbon
#include <qarbon.hpp>

// msdf
#include <generator/msdf/core/range.hpp>
#include <generator/msdf/core/image_ref.hpp>

namespace qtxt {
    // Reconstructs the shape's appearance into output from the distance field sdf
    void RenderSDF(const ImageRef<float, 1>& output, const ImageConstRef<float, 1>& sdf, Range sdfPxRange = 0, float sdThreshold = 0.5f);
    void RenderSDF(const ImageRef<float, 3>& output, const ImageConstRef<float, 1>& sdf, Range sdfPxRange = 0, float sdThreshold = 0.5f);
    void RenderSDF(const ImageRef<float, 1>& output, const ImageConstRef<float, 3>& sdf, Range sdfPxRange = 0, float sdThreshold = 0.5f);
    void RenderSDF(const ImageRef<float, 3>& output, const ImageConstRef<float, 3>& sdf, Range sdfPxRange = 0, float sdThreshold = 0.5f);
    void RenderSDF(const ImageRef<float, 1>& output, const ImageConstRef<float, 4>& sdf, Range sdfPxRange = 0, float sdThreshold = 0.5f);
    void RenderSDF(const ImageRef<float, 4>& output, const ImageConstRef<float, 4>& sdf, Range sdfPxRange = 0, float sdThreshold = 0.5f);

    // Snaps the values of the floating-point bitmaps into one of the 256 values representable in a standard 8-bit bitmap
    void Simulate8Bit(const ImageRef<float, 1>& bitmap);
    void Simulate8Bit(const ImageRef<float, 3>& bitmap);
    void Simulate8Bit(const ImageRef<float, 4>& bitmap);
}

#endif