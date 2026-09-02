#include <msdf/msdf_atlas.hpp>

namespace qtxt {
    const GlyphRecord* MSDFAtlas::getGlyph(uint32_t codepoint) const {
        auto it = glyphs.find(codepoint);
        return it == glyphs.end() ? nullptr : &it->second;
    }

    double MSDFAtlas::getKerning(int codepointA, int codepointB) const {
        auto it = kerning.find({codepointA, codepointB});
        return it == kerning.end() ? 0.0 : it->second * geometryScale;
    }
}