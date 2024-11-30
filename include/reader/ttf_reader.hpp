#ifndef __QONTEXT_TTF_READER_HPP__
#define __QONTEXT_TTF_READER_HPP__

// std
#include <vector>
#include <string>

// reader
#include <reader/reader.hpp>

// filetypes
#include <file_types/truetypefont.hpp>

namespace qtxt {
    template<> struct Reader<TTF> : public Reader<BIGENDIAN> {
        public:
            int32_t ReadFixed() { return ReadInt32(); }
            int16_t ReadFWord() { return ReadInt16(); }
            uint16_t ReadUFWord() { return ReadUInt16(); }
            int16_t ReadLongDateTime() { return ReadInt64(); }

            TTF CreateTTFFromFile(const std::string& path);
            TTF CreateTTFFromFile(const char* path);
        private:
            void LoadTables(TTF& ttf);
            std::vector<std::string> TableOffsets(TTF& ttf);
            void LoadCMAP(TTF& ttf);
            void LoadHEAD(TTF& ttf);
            void LoadMAXP(TTF& ttf);
            void LoadLOCA(TTF& ttf);
            void LoadGLYF(TTF& ttf);
            void LoadHHEA(TTF& ttf);
            void LoadHMTX(TTF& ttf);
            void LoadNAME(TTF& ttf);
            void LoadPOST(TTF& ttf);
            void LoadKERN(TTF& ttf);
    };
}

#endif