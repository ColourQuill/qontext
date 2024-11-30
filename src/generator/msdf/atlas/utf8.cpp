#include <generator/msdf/atlas/utf8.hpp>

namespace qtxt {
    void UTF8Decode(std::vector<uint32_t>& codepoints, const uint8_t* utf8String) {
        bool start = true;
        int rBytes = 0;
        uint32_t codepoint = 0;

        for (const uint8_t* c = utf8String; *c; ++c) {
            if (rBytes > 0) {
                --rBytes;
                if ((*c&0xc0) == 0x80) {
                    codepoint |= (*c&0x3f) << (6 * rBytes);
                }
            } else if (!(*c&0x80)) {
                codepoint = *c;
                rBytes = 0;
            } else if (*c&0x40) {
                int block;
                for (block = 0; ((uint8_t)* c << block)&0x40 && block < 4; ++block);
                if (block < 4) {
                    codepoint = (*c&(0x3f >> block)) << (6 * block);
                    rBytes = block;
                } else {
                    continue;
                }
            } else {
                continue;
            }
            if (!rBytes) {
                if (!(start && codepoint == 0xfeff)) {
                    codepoints.push_back(codepoint);
                }
                start = false;
            }
        }
    }
}