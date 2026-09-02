#include <msdf/atlas/font_geometry.hpp>

// std
#include <ranges>

// msdf
#include <msdf/core/glyph.hpp>

namespace qtxt {
    FontGeometry::FontGeometry(Font& font, Charset charset, double fontScale) : font{font} {
        const FontMetrics& fontMetrics = font.getFontMetrics();
        geometryScale = fontScale / fontMetrics.emSize;
        metrics.emSize = fontMetrics.emSize * geometryScale;
        metrics.ascenderY = fontMetrics.ascenderY * geometryScale;
        metrics.descenderY = fontMetrics.descenderY * geometryScale;
        metrics.lineHeight = fontMetrics.lineHeight * geometryScale;
        metrics.underlineY = fontMetrics.underlineY * geometryScale;
        metrics.underlineThickness = fontMetrics.underlineThickness * geometryScale;

        for (uint32_t codepoint : charset) {
            Glyph& glyph = font.getGlyph(codepoint);
            if (codepoint == 48) {
                capHeight = glyph.getBounds().t;
            }
            if (codepoint == 78) {
                xHeight = glyph.getBounds().t;
            }
            GlyphGeometry glyphGeometry = GlyphGeometry(glyph, codepoint, geometryScale);

            glyphs.insert({codepoint, glyphGeometry});
        }
    }

    double FontGeometry::getGeometryScale() const {
        return geometryScale;
    }
    const FontMetrics& FontGeometry::getMetrics() const {
        return metrics;
    }
    
    const GlyphGeometry* FontGeometry::getGlyph(uint32_t codepoint) const {
        return &glyphs.at(codepoint);
    }

    double FontGeometry::getAdvance(uint32_t codepointA, uint32_t codepointB) const {
        return font.getAdvance(codepointA, codepointB, geometryScale);
    }

    double FontGeometry::getCapHeight() const {
        return capHeight;
    }
    double FontGeometry::getXHeight() const {
        return xHeight;
    }

    const std::map<std::pair<int, int>, int>& FontGeometry::getKerning() const {
        return font.getKerningMap();
    }

    std::vector<GlyphGeometry*> FontGeometry::getGlyphPtrs() const {
        std::vector<GlyphGeometry*> glyphVector;
        for (auto it = glyphs.begin(); it != glyphs.end(); ++it) {
            glyphVector.push_back(const_cast<GlyphGeometry*>(&it->second));
        }
        return glyphVector;
    }

    std::vector<GlyphGeometry> FontGeometry::getGlyphs() const {
        std::vector<GlyphGeometry> glyphVector;
        for (auto it = glyphs.begin(); it != glyphs.end(); ++it) {
            glyphVector.push_back(it->second);
        }
        return glyphVector;
    }
}