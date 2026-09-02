#ifndef __QONTEXT_GENERATE_ATLAS_HPP__
#define __QONTEXT_GENERATE_ATLAS_HPP__

// msdf
#include <msdf/core/image.hpp>
#include <msdf/core/font.hpp>
#include <msdf/atlas/charset.hpp>
#include <msdf/atlas/atlas_config.hpp>

namespace qtxt {
    // forward declarations
    class FontGeometry;
    
    Image<uint8_t, 3> generateMSDFAtlas(FontGeometry& fontGeometry, AtlasConfig config = {});
    Image<uint8_t, 3> generateMSDFAtlas(Font& font, Charset charset, AtlasConfig config = {});
}

#endif