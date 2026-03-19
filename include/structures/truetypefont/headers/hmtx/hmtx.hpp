#ifndef __TTF_HMTX_HPP__
#define __TTF_HMTX_HPP__

// qontext
#include <info/type_defs.hpp>

namespace qtxt {
    struct HMTX { // 'horizontal metrics'
        ~HMTX() {
            if (hMetrics != nullptr) {
                delete[] hMetrics;
            }
            if (leftSideBearing != nullptr) {
                delete[] leftSideBearing;
            }
        }

        struct LongHorMetric {
            uint16_t advanceWidth;
            int16_t leftSideBearing;
        };

        uint32_t offset;
        LongHorMetric* hMetrics{nullptr}; // Number of longHorMetrics comes from 'hhea' table (numOfLongHorMetrics)
        FWord* leftSideBearing{nullptr}; // 'The number of entries in this array is derived from the total number of glyphs minus numOfLongHorMetrics'
    };
}

#endif