#ifndef __QONTEXT_MSDF_ATLAS_HPP__
#define __QONTEXT_MSDF_ATLAS_HPP__

// msdf
#include <generator/msdf/core/image.hpp>
#include <generator/msdf/core/msdf_font.hpp>
#include <generator/msdf/atlas/types.hpp>
#include <generator/msdf/atlas/charset.hpp>

namespace qtxt {
    // forward declarations
    class FontGeometry;

    struct AtlasConfig {
        DimensionsConstraint constraint = DimensionsConstraint::SQUARE;
        double minScale = 24.0;
        double pxRange = 2.0;
        double maxCornerAngle = 3.0;
        int threadCount = 4.0;
        bool alignPxOrigin = true;
    };
    
    Image<uint8_t, 1> GenerateSDFAtlas(FontGeometry& fontGeometry, AtlasConfig config = {});
    Image<uint8_t, 1> GenerateSDFAtlas(MSDFFont& font, Charset charset, AtlasConfig config = {});

    Image<uint8_t, 1> GeneratePSDFAtlas(FontGeometry& fontGeometry, AtlasConfig config = {});
    Image<uint8_t, 1> GeneratePSDFAtlas(MSDFFont& font, Charset charset, AtlasConfig config = {});
    
    Image<uint8_t, 3> GenerateMSDFAtlas(FontGeometry& fontGeometry, AtlasConfig config = {});
    Image<uint8_t, 3> GenerateMSDFAtlas(MSDFFont& font, Charset charset, AtlasConfig config = {});

    Image<uint8_t, 4> GenerateMTSDFAtlas(FontGeometry& fontGeometry, AtlasConfig config = {});
    Image<uint8_t, 4> GenerateMTSDFAtlas(MSDFFont& font, Charset charset, AtlasConfig config = {});
}

#endif