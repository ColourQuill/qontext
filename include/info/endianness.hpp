#ifndef __QONTEXT_ENDIANNESS_HPP__
#define __QONTEXT_ENDIANNESS_HPP__

// std
#include <cstdint>

namespace qtxt {
    enum class Endianness {
        BIGENDIAN,
        LITTLEENDIAN
    };

    struct BIGENDIAN {};
    struct LITTLEENDIAN {};

    Endianness GetEndianness();
    void EndiannessConverter(Endianness input, Endianness output, unsigned char* value, unsigned long bytes);
    std::uintmax_t CombineBytes(unsigned char* value, unsigned long bytes);
    void SeparateBytes(std::uintmax_t value, unsigned char* output, unsigned long bytes);
}

#endif