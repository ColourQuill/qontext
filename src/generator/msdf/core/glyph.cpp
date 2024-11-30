#include <generator/msdf/core/glyph.hpp>

namespace qtxt {
    Glyph::Glyph(Shape shape, GlyphMetrics metrics, int index) : shape{shape}, metrics{metrics}, index{index} {}
}