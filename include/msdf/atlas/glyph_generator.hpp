#ifndef __QONTEXT_GLYPH_GENERATOR_HPP__
#define __QONTEXT_GLYPH_GENERATOR_HPP__

// msdf
#include <msdf/core/scanline.hpp>
#include <msdf/atlas/glyph_geometry.hpp>
#include <msdf/atlas/atlas_generator.hpp>

namespace qtxt {
    void ScanlineGenerator(Image<float, 1>& output, const GlyphGeometry& glyph, const GeneratorAttributes& attribs);
    void GlyphGenerator(Image<float, 3>& output, const GlyphGeometry& glyph, const GeneratorAttributes& attribs);
}

#endif