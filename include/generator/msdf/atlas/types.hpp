#ifndef __QONTEXT_TYPES_HPP__
#define __QONTEXT_TYPES_HPP__

// std
#include <cstdint>

namespace qtxt {
    enum class ImageType {
        HARD_MASK,
        SOFT_MASK,
        SDF,
        PSDF,
        MSDF,
        MTSDF
    };

    enum class GlyphIdentifierType {
        GLYPH_INDEX,
        UNICODE_CODEPOINT
    };

    enum class YDirection {
        BOTTOM_UP,
        TOP_DOWN
    };

    enum class PackingStyle {
        TIGHT,
        GRID
    };

    enum class DimensionsConstraint {
        NONE,
        SQUARE,
        EVEN_SQUARE,
        MULTIPLE_OF_FOUR_SQUARE,
        POWER_OF_TWO_RECTANGLE,
        POWER_OF_TWO_SQUARE
    };
}

#endif