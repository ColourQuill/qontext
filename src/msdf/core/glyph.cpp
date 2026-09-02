#include <msdf/core/glyph.hpp>

namespace qtxt {
    Glyph::Glyph(Shape shape, GlyphMetrics metrics, int index)
        : index(index), shape(std::move(shape)), metrics(metrics) {}

    double Glyph::getAdvance() const {
        return metrics.advanceWidth;
    }
    double Glyph::getLeftBearing() const {
        return metrics.leftSideBearing;
    }
    Bounds Glyph::getBounds() const {
        return shape.getBounds();
    }
}