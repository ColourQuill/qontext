#ifndef __QONTEXT_GLYPH_HPP__
#define __QONTEXT_GLYPH_HPP__

// msdf
#include <msdf/core/glyph_metrics.hpp>
#include <msdf/core/bounds.hpp>
#include <msdf/core/shape/shape.hpp>

namespace qtxt {
    class Glyph {
        public:
            Glyph(Shape shape, GlyphMetrics metrics, int index);

            int index;
            Shape shape;
            GlyphMetrics metrics;

            double getAdvance() const;
            double getLeftBearing() const;
            Bounds getBounds() const;
        private:
    };
}

#endif