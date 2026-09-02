#ifndef __QONTEXT_TTF_GLYF_HPP__
#define __QONTEXT_TTF_GLYF_HPP__

// qontext
#include <truetypefont/headers/glyf/glyf_glyph.hpp>

namespace qtxt {
    struct GLYF {
        ~GLYF() {
            for (int i = 0; i < numGlyphs; ++i) {
                if (glyphs[i] != nullptr) {
                    delete glyphs[i];
                    glyphs[i] = nullptr;
                }
            }
            delete[] glyphs;
            glyphs = nullptr;
        }

        uint32_t offset;
        TTFGlyph** glyphs{nullptr};
        uint32_t numGlyphs;
    };
}

#endif