#include <info/endianness.hpp>

// std
#include <algorithm>

namespace qtxt {
    Endianness GetEndianness() {
        const int value {0x01};
        const void* address {static_cast<const void*>(&value)};
        const unsigned char* least_significant_address{static_cast<const unsigned char*>(address)};
        return (*least_significant_address == 0x01) ? Endianness::LITTLEENDIAN : Endianness::BIGENDIAN;
    }

    void EndiannessConverter(Endianness input, Endianness output, unsigned char* value, unsigned long bytes) {
        if (input == output) {
            std::reverse(value, value + bytes);
        }
    }

    std::uintmax_t CombineBytes(unsigned char* value, unsigned long bytes) {
        std::uintmax_t combinedValue = 0;
        for (size_t i = 0; i < bytes; ++i) {
            combinedValue = (combinedValue << 8) | value[i];
        }
        return combinedValue;
    }
    void SeparateBytes(std::uintmax_t value, unsigned char* output, unsigned long bytes) {
        for (size_t i = 0; i < bytes; ++i) {
            output[bytes - 1 - i] = static_cast<unsigned char>(value & 0xFF);
            value >>= 8;
        }
    }
}