#ifndef __QONTEXT_TTF_HHEA_HPP__
#define __QONTEXT_TTF_HHEA_HPP__

// qontext
#include <info/type_defs.hpp>

namespace qtxt {
    struct HHEA { // 'horizontal header'
        uint32_t offset;
        Fixed version; // '0x00010000 (1.0)'
        FWord ascent; // 'Distance from baseline of highest ascender'
        FWord descent; // 'Distance from baseline of lowest descender'
        FWord lineGap; // 'typographic line gap'
        uFWord advanceWidthMax; // 'must be consistent with horizontal metric'
        FWord minLeftSideBearing; // 'must be consistent with horizontal metric'
        FWord minRightSideBearing; // 'must be consistent with horizontal metric'
        FWord xMaxExtent; // 'max(lsb + (xMax - xMin))'
        int16_t caretSlopeRise; // 'used to calculate the slope of the caret (rise/run) set to 1 for vertical caret'
        int16_t caretSlopeRun; // '0 for vertical'
        FWord caretOffset; // 'set value to 0 for non-slanted fonts'
        int64_t reserved; // 'set value to 0'
        int16_t metricDataFormat; // '0 for current format'
        uint16_t numOfLongHorMetrics; // 'number of advance widths in metrics table'
    };
}

#endif