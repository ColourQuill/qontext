#ifndef __QONTEXT_MSDF_FONT_HPP__
#define __QONTEXT_MSDF_FONT_HPP__

// std
#include <string>
#include <cstdint>
#include <vector>
#include <map>
#include <unordered_map>

// msdf
#include <generator/msdf/core/shape.hpp>

namespace qtxt {
    // forward declarations
    class Glyph;

    struct FontMetrics {
        double emSize;
        double ascenderY;
        double descenderY;
        double lineHeight;
        double underlineY;
        double underlineThickness;
    };

    struct FontVariationAxis {
        const char* name;
        double minValue;
        double maxValue;
        double defaultValue;
    };

    enum class FontCoordinateScaling {
        NONE,
        EM_NORMALIZED
    };

    class MSDFFont {
        public:
            MSDFFont(const std::string& path);
            ~MSDFFont();

            Glyph& GetGlyph(uint32_t unicode);
            std::vector<Glyph>& GetGlyphs();

            double GetKerning(uint32_t codepointA, uint32_t codepointB, double geometryScale = 1.0);
            double GetAdvance(uint32_t codepointA, uint32_t codepointB, double geometryScale = 1.0);
            const FontMetrics& GetFontMetrics() const;

            const std::map<std::pair<int, int>, int>& GetKerningMap() const;
        private:
            std::vector<Glyph> glyphs;
            std::map<uint32_t, uint32_t> mapping;
            std::map<std::pair<int, int>, int> kerning;

            std::string name;

            FontMetrics metrics;

            Shape whitespaceShape{};
    };
}

#endif