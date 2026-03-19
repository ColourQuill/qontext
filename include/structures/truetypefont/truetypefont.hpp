#ifndef __TRUE_TYPE_FONT_HPP__
#define __TRUE_TYPE_FONT_HPP__

// qontext
#include <truetypefont/headers/cmap/cmap.hpp>
#include <truetypefont/headers/glyf/glyf.hpp>
#include <truetypefont/headers/head/head.hpp>
#include <truetypefont/headers/hhea/hhea.hpp>
#include <truetypefont/headers/hmtx/hmtx.hpp>
#include <truetypefont/headers/kern/kern.hpp>
#include <truetypefont/headers/loca/loca.hpp>
#include <truetypefont/headers/maxp/maxp.hpp>
#include <truetypefont/headers/name/name.hpp>
#include <truetypefont/headers/post/post.hpp>

namespace qtxt {
    class TrueTypeFont {
        public:
            
        private:
            CMAP cmap;
            GLYF glyf;
            HEAD head;
            HHEA hhea;
            HMTX hmtx;
            LOCA loca;
            MAXP maxp;
            NAME name;
            POST post;
            KERN kern;
    };
}

#endif