#ifndef __QONTEXT_MSDF_ATLAS_HPP__
#define __QONTEXT_MSDF_ATLAS_HPP__

// std
#include <cstdint>
#include <map>

// msdf
#include <msdf/core/image.hpp>
#include <msdf/core/font_metrics.hpp>
#include <msdf/atlas/types.hpp>

namespace qtxt {
    struct GlyphRecord {
        uint32_t codepoint;
        double advance;
        double planeL, planeB, planeR, planeT;
        double atlasL, atlasB, atlasR, atlasT;
    };
    struct MSDFAtlas {
        Image<uint8_t, 3> atlas;
        FontMetrics metrics;
        double geometryScale;
        std::map<uint32_t, GlyphRecord> glyphs;
        std::map<std::pair<int, int>, int> kerning;

        DimensionsConstraint constraint;
        double minScale;
        double pxRange;
        double maxCornerAngle;
        bool alignPxOrigin;
        int spacing;

        const GlyphRecord* getGlyph(uint32_t codepoint) const;
        double getKerning(int codepointA, int codepointB) const;
    };
}

#endif