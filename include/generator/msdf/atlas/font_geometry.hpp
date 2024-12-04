#ifndef __QONTEXT_FONT_GEOMETRY_HPP__
#define __QONTEXT_FONT_GEOMETRY_HPP__

// std
#include <map>

// msdf
#include <generator/msdf/core/msdf_font.hpp>
#include <generator/msdf/atlas/charset.hpp>
#include <generator/msdf/atlas/glyph_geometry.hpp>

namespace qtxt {
    class FontGeometry {
        public:
            FontGeometry(MSDFFont& font, Charset charset, double fontScale = 1.0);

            double GetGeometryScale() const;
            const FontMetrics& GetMetrics() const;
            
            const GlyphGeometry* GetGlyph(uint32_t codepoint) const;

            double GetAdvance(uint32_t codepointA, uint32_t codepointB) const;
            double GetCapHeight() const;
            double GetXHeight() const;

            const std::map<std::pair<int, int>, int>& GetKerning() const;

            std::vector<GlyphGeometry*> GetGlyphPtrs() const;
            std::vector<GlyphGeometry> GetGlyphs() const;

        private:
            MSDFFont& font;
            FontMetrics metrics;

            std::map<uint32_t, GlyphGeometry> glyphs;

            double geometryScale;

            double capHeight{0.0};
            double xHeight{0.0};
    };
}

#endif