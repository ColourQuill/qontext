#ifndef __QONTEXT_UTF8_HPP__
#define __QONTEXT_UTF8_HPP__

// std
#include <vector>
#include <cstdint>

// msdf
#include <msdf/atlas/types.hpp>

namespace qtxt {
    void decodeUTF8(std::vector<uint32_t>& codepoints, const uint8_t* utf8String);
}

#endif