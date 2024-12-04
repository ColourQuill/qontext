#include <generator/msdf/atlas/font_geometry.hpp>

// std
#include <ranges>

// msdf
#include <generator/msdf/core/glyph.hpp>

namespace qtxt {
    FontGeometry::FontGeometry(MSDFFont& font, Charset charset, double fontScale) : font{font} {
        const FontMetrics& fontMetrics = font.GetFontMetrics();
        geometryScale = fontScale / fontMetrics.emSize;
        metrics.emSize = fontMetrics.emSize * geometryScale;
        metrics.ascenderY = fontMetrics.ascenderY * geometryScale;
        metrics.descenderY = fontMetrics.descenderY * geometryScale;
        metrics.lineHeight = fontMetrics.lineHeight * geometryScale;
        metrics.underlineY = fontMetrics.underlineY * geometryScale;
        metrics.underlineThickness = fontMetrics.underlineThickness * geometryScale;

        for (uint32_t codepoint : charset) {
            Glyph& glyph = font.GetGlyph(codepoint);
            if (codepoint == 48) {
                capHeight = glyph.GetBounds().t;
            }
            if (codepoint == 78) {
                xHeight = glyph.GetBounds().t;
            }
            GlyphGeometry glyphGeometry = GlyphGeometry(glyph, codepoint, geometryScale);

            glyphs.insert({codepoint, glyphGeometry});
        }
    }

    double FontGeometry::GetGeometryScale() const {
        return geometryScale;
    }
    const FontMetrics& FontGeometry::GetMetrics() const {
        return metrics;
    }
    
    const GlyphGeometry* FontGeometry::GetGlyph(uint32_t codepoint) const {
        return &glyphs.at(codepoint);
    }

    double FontGeometry::GetAdvance(uint32_t codepointA, uint32_t codepointB) const {
        return font.GetAdvance(codepointA, codepointB, geometryScale);
    }

    double FontGeometry::GetCapHeight() const {
        return capHeight;
    }
    double FontGeometry::GetXHeight() const {
        return xHeight;
    }

    const std::map<std::pair<int, int>, int>& FontGeometry::GetKerning() const {
        return font.GetKerningMap();
    }

    std::vector<GlyphGeometry*> FontGeometry::GetGlyphPtrs() const {
        std::vector<GlyphGeometry*> glyphVector;
        for (auto it = glyphs.begin(); it != glyphs.end(); ++it) {
            glyphVector.push_back(const_cast<GlyphGeometry*>(&it->second));
        }
        return glyphVector;
    }

    std::vector<GlyphGeometry> FontGeometry::GetGlyphs() const {
        std::vector<GlyphGeometry> glyphVector;
        for (auto it = glyphs.begin(); it != glyphs.end(); ++it) {
            glyphVector.push_back(it->second);
        }
        return glyphVector;
    }
}