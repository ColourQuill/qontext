#ifndef __QONTEXT_TTF_CMAP_HPP__
#define __QONTEXT_TTF_CMAP_HPP__

// qontext
#include <truetypefont/headers/cmap/cmap_subtable.hpp>

namespace qtxt {
    struct CMAP {
        ~CMAP() {
            if (subtables != nullptr) {
                delete[] subtables;
            }
        }

        uint32_t offset;
        uint16_t version; // 'Version number (set to zero)'
        uint16_t numberSubtables; // 'Number of encoding subtables'
        Subtable* subtables{nullptr};
    };
}

#endif