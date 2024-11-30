#ifndef __QONTEXT_GLYPH_GENERATORS_HPP__
#define __QONTEXT_GLYPH_GENERATORS_HPP__

// msdf
#include <generator/msdf/core/scanline.hpp>
#include <generator/msdf/atlas/glyph_geometry.hpp>
#include <generator/msdf/atlas/atlas_generator.hpp>

namespace qtxt {
    #define MSDF_ATLAS_GLYPH_FILL_RULE FILL_NONZERO

    void ScanlineGenerator(const ImageRef<float, 1>& output, const GlyphGeometry& glyph, const GeneratorAttributes& attribs);
    void SDFGenerator(const ImageRef<float, 1>& output, const GlyphGeometry& glyph, const GeneratorAttributes& attribs);
    void PSDFGenerator(const ImageRef<float, 1>& output, const GlyphGeometry& glyph, const GeneratorAttributes& attribs);
    void MSDFGenerator(const ImageRef<float, 3>& output, const GlyphGeometry& glyph, const GeneratorAttributes& attribs);
    void MTSDFGenerator(const ImageRef<float, 4>& output, const GlyphGeometry& glyph, const GeneratorAttributes& attribs);
}

#endif