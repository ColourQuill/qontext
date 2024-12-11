#ifndef __QONTEXT_READER_HPP__
#define __QONTEXT_READER_HPP__

// core
#include <core/info/endianness.hpp>
#include <core/info/file_info.hpp>

// std
#include <vector>
#include <string>

namespace qtxt {
    template<typename T> struct Reader {};
    
    template<> struct Reader<BINARY> {
        void ReadFile(const std::string& path);
        void ReadFile(const char* path);
        void ClearBuffer();

        void SkipBytes(const unsigned long long bytes);
        void SetCurrentByte(const unsigned long long bytes);
        void ResetCurrentByte();

        unsigned long long GetPosition() { return position; }

        char* file{nullptr};

        protected:
            unsigned long long position;
            unsigned long long size;
            Endianness endianness;
    };
    template<> struct Reader<TEXT> {
        void ReadFile(const std::string& path);
        void ReadFile(const char* path);
        void ClearBuffer();

        void SkipLines(unsigned long long lines);
        void SetCurrentLine(unsigned long long line);
        void ResetCurrentLine();
        std::string ReadLine();

        unsigned long long GetPosition() { return position; }

        std::vector<std::string> buffer;

        protected:
            unsigned long long position = 0;
            unsigned long long totalLines = 0;
    };

    template<> struct Reader<BIGENDIAN> : public Reader<BINARY> {
        uint8_t ReadUInt8();
        uint16_t ReadUInt16();
        uint32_t ReadUInt32();
        uint64_t ReadUInt64();

        int8_t ReadInt8();
        int16_t ReadInt16();
        int32_t ReadInt32();
        int64_t ReadInt64();
        
        char ReadChar();
        signed char ReadSChar();
        unsigned char ReadUChar();

        wchar_t ReadWChar();
        char16_t ReadChar16();
        char32_t ReadChar32();

        std::string ReadString(unsigned long int bytes);

        short ReadShort();
        unsigned short ReadUShort();

        int ReadInt();
        unsigned int ReadUInt();

        long ReadLong();
        unsigned long ReadULong();

        long long ReadLongLong();
        unsigned long long ReadULongLong();

        float ReadFloat();
        double ReadDouble();

        bool ReadBool();

        double ReadFixedPoint(unsigned long int integerBits, unsigned long int fractionalBits);
    };

    template<> struct Reader<LITTLEENDIAN> : public Reader<BINARY> {
        uint8_t ReadUInt8();
        uint16_t ReadUInt16();
        uint32_t ReadUInt32();
        uint64_t ReadUInt64();

        int8_t ReadInt8();
        int16_t ReadInt16();
        int32_t ReadInt32();
        int64_t ReadInt64();
        
        char ReadChar();
        signed char ReadSChar();
        unsigned char ReadUChar();

        wchar_t ReadWChar();
        char16_t ReadChar16();
        char32_t ReadChar32();

        std::string ReadString(unsigned long int bytes);

        short ReadShort();
        unsigned short ReadUShort();

        int ReadInt();
        unsigned int ReadUInt();

        long ReadLong();
        unsigned long ReadULong();

        long long ReadLongLong();
        unsigned long long ReadULongLong();

        float ReadFloat();
        double ReadDouble();

        bool ReadBool();

        double ReadFixedPoint(unsigned long int integerBits, unsigned long int fractionalBits);
    };
};

#endif