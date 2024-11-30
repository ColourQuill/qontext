#include <generator/msdf/atlas/charset.hpp>

namespace qtxt {
    static Charset CreateASCIICharset() {
        Charset ascii;
        for (uint32_t codepoint = 0x20; codepoint < 0x7f; ++codepoint) {
            ascii.Add(codepoint);
        }
        return ascii;
    }

    const Charset Charset::ASCII = CreateASCIICharset();

    void Charset::Add(uint32_t codepoint) {
        codepoints.insert(codepoint);
    }
    void Charset::Remove(uint32_t codepoint) {
        codepoints.erase(codepoint);
    }

    size_t Charset::size() const {
        return codepoints.size();
    }
    bool Charset::empty() const {
        return codepoints.empty();
    }
    std::set<uint32_t>::const_iterator Charset::begin() const {
        return codepoints.begin();
    }
    std::set<uint32_t>::const_iterator Charset::end() const {
        return codepoints.end();
    }
}