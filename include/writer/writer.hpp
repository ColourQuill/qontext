#ifndef __QONTEXT_WRITER_HPP__
#define __QONTEXT_WRITER_HPP__

// core
#include <core/info/endianness.hpp>
#include <core/info/file_info.hpp>

// std
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

namespace qtxt {
    template<typename T> struct Writer {};

    template<> struct Writer<BINARY> {
        void OpenFile(const std::string& path);
        void OpenFile(const char* path);
        void CloseFile();
        void ClearBuffer();

        void WriteBytes(const char* data, unsigned long long size);
        void WriteByte(const char byte);

        unsigned long long GetPosition() { return position; }

        protected:
            unsigned long long position = 0;
            Endianness endianness;
            std::ofstream file;
    };

    template<> struct Writer<TEXT> {
        void OpenFile(const std::string& path);
        void OpenFile(const char* path);
        void CloseFile();
        void SaveFile(const std::string& path);
        void SaveFile(const char* path);
        void ClearBuffer();

        void WriteLine(const std::string& line, long long lineIndex = -1);
        void WriteLines(const std::vector<std::string>& lines);

        unsigned long long GetPosition() { return position; }

        std::vector<std::string> buffer;

        protected:
            unsigned long long position = 0;
            std::fstream file;
    };

    template<> struct Writer<BIGENDIAN> : public Writer<BINARY> {
        void WriteUInt8(uint8_t value);
        void WriteUInt16(uint16_t value);
        void WriteUInt32(uint32_t value);
        void WriteUInt64(uint64_t value);

        void WriteInt8(int8_t value);
        void WriteInt16(int16_t value);
        void WriteInt32(int32_t value);
        void WriteInt64(int64_t value);

        void WriteChar(char value);
        void WriteString(const std::string& value);

        void WriteShort(short value);
        void WriteUShort(unsigned short value);

        void WriteInt(int value);
        void WriteUInt(unsigned int value);

        void WriteLong(long value);
        void WriteULong(unsigned long value);

        void WriteLongLong(long long value);
        void WriteULongLong(unsigned long long value);

        void WriteFloat(float value);
        void WriteDouble(double value);

        void WriteBool(bool value);

        void WriteFixedPoint(double value, unsigned long int integerBits, unsigned long int fractionalBits);
    };

    template<> struct Writer<LITTLEENDIAN> : public Writer<BINARY> {
        void WriteUInt8(uint8_t value);
        void WriteUInt16(uint16_t value);
        void WriteUInt32(uint32_t value);
        void WriteUInt64(uint64_t value);

        void WriteInt8(int8_t value);
        void WriteInt16(int16_t value);
        void WriteInt32(int32_t value);
        void WriteInt64(int64_t value);

        void WriteChar(char value);
        void WriteString(const std::string& value);

        void WriteShort(short value);
        void WriteUShort(unsigned short value);

        void WriteInt(int value);
        void WriteUInt(unsigned int value);

        void WriteLong(long value);
        void WriteULong(unsigned long value);

        void WriteLongLong(long long value);
        void WriteULongLong(unsigned long long value);

        void WriteFloat(float value);
        void WriteDouble(double value);

        void WriteBool(bool value);

        void WriteFixedPoint(double value, unsigned long int integerBits, unsigned long int fractionalBits);
    };
}

#endif