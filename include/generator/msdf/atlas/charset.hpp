#ifndef __QONTEXT_CHARSET_HPP__
#define __QONTEXT_CHARSET_HPP__

// std
#include <cstdlib>
#include <cstdint>
#include <set>

namespace qtxt {
    class Charset {
        public:
            static const Charset ASCII;

            void Add(uint32_t codepoint);
            void Remove(uint32_t codepoint);

            size_t size() const;
            bool empty() const;
            std::set<uint32_t>::const_iterator begin() const;
            std::set<uint32_t>::const_iterator end() const;

            bool Load(const char* file, bool disableCharLiterals = false);
            bool Parse(const char* str, size_t strLength, bool disableCharLiterals = false);
        private:
            std::set<uint32_t> codepoints;
    };
}

#endif