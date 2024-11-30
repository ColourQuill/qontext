#ifndef __QONTEXT_GLYPH_BOX_HPP__
#define __QONTEXT_GLYPH_BOX_HPP__

// msdf
#include <generator/msdf/atlas/rectangle.hpp>

namespace qtxt {
    struct GlyphBox {
        int index{0};
        double advance{0};
        struct {
            double l{0.0}, b{0.0}, r{0.0}, t{0.0};
        } bounds;
        Rectangle rect{};
    };
}

#endif