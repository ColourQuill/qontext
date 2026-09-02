#ifndef __QONTEXT_TTF_POST_HPP__
#define __QONTEXT_TTF_POST_HPP__

// qontext
#include <info/type_defs.hpp>

namespace qtxt {
    struct POST { // 'PostScript'
        uint32_t offset;
        Fixed format; // 'Format of this table' (check documentation for information about formats: https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6post.html)
        Fixed italicAngle; // 'Italic angle in degrees'
        FWord underlinePosition; // 'Underline position'
        FWord underlineThickness; // 'Underline thickness'
        uint32_t isFixedPitch; // 'set to 1 if font is monospaced and 0 otherwise'
        uint32_t minMemType42; // 'Minimum memory usage when a TrueType font is downloaded as a Type 42 font'
        uint32_t maxMemType42; // 'Maximum memory usage when a TrueType font is downloaded as a Type 42 font'
        uint32_t minMemType1; // 'Minimum memory usage when a TrueType font is downloaded as a Type 1 font'
        uint32_t maxMemType1; // 'Maximum memory usage when a TrueType font is downloaded as a Type 1 font'
    };
}

#endif