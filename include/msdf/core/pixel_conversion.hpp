#ifndef __QONTEXT_PIXEL_CONVERSION_HPP__
#define __QONTEXT_PIXEL_CONVERSION_HPP__

// std
#include <cstdint>

// msdf
#include <msdf/core/arithmetic.hpp>

namespace qtxt {
    inline uint8_t pixelFloatToByte(float x) {
        return uint8_t(~int(255.5f - 255.0f * clamp(x)));
    }

    inline float pixelByteToFloat(uint8_t x) {
        return float(1.0f / 255.0f) * float(x); 
    }
}

#endif