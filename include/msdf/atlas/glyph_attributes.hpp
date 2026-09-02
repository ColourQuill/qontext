#ifndef __QONTEXT_GLYPH_ATTRIBUTES_HPP__
#define __QONTEXT_GLYPH_ATTRIBUTES_HPP__

// msdf
#include <msdf/core/range.hpp>
#include <msdf/atlas/padding.hpp>

namespace qtxt {
    struct GlyphAttributes {
            double scale;
            Range range;
            Padding innerPadding, outerPadding;
            double miterLimit;
            bool pxAlignOriginX, pxAlignOriginY;
        };
}

#endif