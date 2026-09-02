#ifndef __QONTEXT_TRUE_TYPE_FONT_HPP__
#define __QONTEXT_TRUE_TYPE_FONT_HPP__

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
#include <utils/reader.hpp>

namespace qtxt {
    class TrueTypeFont {
        public:
            TrueTypeFont(const std::string& path);

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
        private:
            void Init(Reader<BIGENDIAN>& reader);
            // required
            void InitCMAP(Reader<BIGENDIAN>& reader);
            void InitHEAD(Reader<BIGENDIAN>& reader);
            void InitMAXP(Reader<BIGENDIAN>& reader);
            void InitLOCA(Reader<BIGENDIAN>& reader);
            void InitGLYF(Reader<BIGENDIAN>& reader);
            void InitHHEA(Reader<BIGENDIAN>& reader);
            void InitHMTX(Reader<BIGENDIAN>& reader);
            void InitNAME(Reader<BIGENDIAN>& reader);
            void InitPOST(Reader<BIGENDIAN>& reader);

            // optional
            void InitKERN(Reader<BIGENDIAN>& reader);
    };
}

#endif