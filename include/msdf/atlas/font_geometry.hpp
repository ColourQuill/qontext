#ifndef __QONTEXT_FONT_GEOMETRY_HPP__
#define __QONTEXT_FONT_GEOMETRY_HPP__

// std
#include <map>

// msdf
#include <msdf/core/font.hpp>
#include <msdf/atlas/charset.hpp>
#include <msdf/atlas/glyph_geometry.hpp>

namespace qtxt {
    class FontGeometry {
        public:
            FontGeometry(Font& font, Charset charset, double fontScale = 1.0);

            double getGeometryScale() const;
            const FontMetrics& getMetrics() const;
            
            const GlyphGeometry* getGlyph(uint32_t codepoint) const;

            double getAdvance(uint32_t codepointA, uint32_t codepointB) const;
            double getCapHeight() const;
            double getXHeight() const;

            const std::map<std::pair<int, int>, int>& getKerning() const;

            std::vector<GlyphGeometry*> getGlyphPtrs() const;
            std::vector<GlyphGeometry> getGlyphs() const;
        private:
            Font& font;
            FontMetrics metrics;

            std::map<uint32_t, GlyphGeometry> glyphs;

            double geometryScale;

            double capHeight{0.0};
            double xHeight{0.0};
    };
}

#endif