#ifndef __TTF_LOCA_HPP__
#define __TTF_LOCA_HPP__

// qontext
#include <info/type_defs.hpp>

namespace qtxt {
    struct LOCA { // 'index to location'
        ~LOCA() {
            if (glyphOffsets != nullptr) {
                delete[] glyphOffsets;
            }
        }
        uint32_t offset;
        uint32_t* glyphOffsets{nullptr}; // num = num glyphs
    };
}

#endif