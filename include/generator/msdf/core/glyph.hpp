#ifndef __QONTEXT_GLYPH_HPP__
#define __QONTEXT_GLYPH_HPP__

// msdf
#include <generator/msdf/core/bounds.hpp>
#include <generator/msdf/core/shape.hpp>

namespace qtxt {
    struct GlyphMetrics {
        double advanceWidth;
        double leftSideBearing;
    };
    class Glyph {
        public:
            Glyph(Shape shape, GlyphMetrics metrics, int index);

            int index;
            Shape shape;
            GlyphMetrics metrics;

            double GetAdvance() { return metrics.advanceWidth; }
            double GetLeftBearing() { return metrics.leftSideBearing; }
            Bounds GetBounds() { return shape.GetBounds(); }
    };
}

#endif