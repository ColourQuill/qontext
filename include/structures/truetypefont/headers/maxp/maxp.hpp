#ifndef __QONTEXT_TTF_MAXP_HPP__
#define __QONTEXT_TTF_MAXP_HPP__

// qontext
#include <info/type_defs.hpp>

namespace qtxt {
    struct MAXP { // 'maximum profile'
        uint32_t offset;
        Fixed version; // '0x00010000 (1.0)'
        uint16_t numGlyphs; // 'the number of glyphs in the font'
        uint16_t maxPoints; // 'points in non-compound glyph'
        uint16_t maxContours; // 'contours in non-compound glyph'
        uint16_t maxComponentPoints; // 'points in compound glyph'
        uint16_t maxComponentContours; // 'contours in compound glyph'
        uint16_t maxZones; // 'set to 2'
        uint16_t maxTwilightPoints; // 'points used in Twilight Zone (Z0)'
        uint16_t maxStorage; // 'number of Storage Area locations'
        uint16_t maxFunctionDefs; // 'number of FDEFs'
        uint16_t maxInstructionDefs; // 'number of IDEFs'
        uint16_t maxStackElements; // 'maximum stack depth'
        uint16_t maxSizeOfInstructions; // 'byte count for glyph instructions'
        uint16_t maxComponentElements; // 'number of glyphs referenced at top level'
        uint16_t maxComponentDepth; // 'levels of recursion, set to 0 if font only has simple glyphs'
    };
}

#endif