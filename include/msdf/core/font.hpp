#ifndef __QONTEXT_FONT_HPP__
#define __QONTEXT_FONT_HPP__

// std
#include <string>
#include <vector>
#include <cstdint>
#include <map>

// msdf/core
#include <msdf/core/font_metrics.hpp>
#include <msdf/core/font_variation_axis.hpp>
#include <msdf/core/font_coordinate_scaling.hpp>

namespace qtxt {
    // forward declarations;
    class Glyph;

    class Font {
        public:
            Font(const std::string& path);
            ~Font();

            Glyph& getGlyph(uint32_t unicode);
            std::vector<Glyph>& getGlyphs();

            double getKerning(uint32_t codepointA, uint32_t codepointB, double geometryScale = 1.0);
            double getAdvance(uint32_t codepointA, uint32_t codepointB, double geometryScale = 1.0);
            const FontMetrics& getFontMetrics() const;

            const std::map<std::pair<int, int>, int>& getKerningMap() const;
        private:
            std::vector<Glyph> glyphs;
            std::map<uint32_t, uint32_t> mapping;
            std::map<std::pair<int, int>, int> kerning;

            std::string name;

            FontMetrics metrics;
    };
}

#endif