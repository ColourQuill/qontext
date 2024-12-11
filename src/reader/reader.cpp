#include <reader/reader.hpp>

// std
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstring>

namespace qtxt {
    // *********************************************************************************************
    //                          BINARY READER STRUCT DEFINITIONS
    // *********************************************************************************************

    // Reads file contents to a buffer
    void Reader<BINARY>::ReadFile(const std::string& path) {
        ReadFile(path.c_str());
    }
    // Reads file contents to a buffer
    void Reader<BINARY>::ReadFile(const char* path) {
        std::ifstream file(path, std::ios::binary | std::ios::ate);

        if (!file.is_open()) {
            std::runtime_error("Failed to open file: " + std::string(path));
        }
        endianness = GetEndianness();

        std::streamsize size = file.tellg();
        this->size = size;
        file.seekg(0, std::ios::beg);

        this->file = new char[size];
        file.read(this->file, size);

        if (!file) {
            delete[] this->file;
            throw std::runtime_error("Failed to read the entire file: " + std::string(path));
        }

        position = 0;
        file.close();
    }
    // Clears the buffer
    void Reader<BINARY>::ClearBuffer() {
        delete[] file;
    }
    // Skips bytes in buffer
    void Reader<BINARY>::SkipBytes(const unsigned long long bytes) {
        position += bytes;
        if (position >= size) {
            position = size - 1;
        }
    }
    // Sets current position in buffer
    void Reader<BINARY>::SetCurrentByte(const unsigned long long byte) {
        position = byte;
        if (position >= size) {
            position = size - 1;
        }
    }
    // Resets position in buffer
    void Reader<BINARY>::ResetCurrentByte() {
        position = 0;
    }

    // *********************************************************************************************
    //                             TEXT READER STRUCT DEFINITIONS
    // *********************************************************************************************

    void Reader<TEXT>::ReadFile(const std::string& path) {
        ReadFile(path.c_str());
    }
    void Reader<TEXT>::ReadFile(const char* path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::runtime_error("Failed to open file: " + std::string(path));
        }

        buffer.clear();
        std::string line;
        while (std::getline(file, line)) {
            buffer.push_back(line);
        }
        file.close();

        totalLines = buffer.size();
        position = 0;
    }
    void Reader<TEXT>::ClearBuffer() {
        buffer.clear();
        position = 0;
        totalLines = 0;
    }

    void Reader<TEXT>::SkipLines(unsigned long long lines) {
        position += lines;
        if (position >= totalLines) {
            position = totalLines - 1;
        }
    }
    void Reader<TEXT>::SetCurrentLine(unsigned long long line) {
        position = line;
        if (position >= totalLines) {
            position = totalLines - 1;
        }
    }
    void Reader<TEXT>::ResetCurrentLine() {
        position = 0;
    }
    std::string Reader<TEXT>::ReadLine() {
        if (position < totalLines) {
            return buffer[position++];
        } else {
            return buffer[totalLines - 1];
        }
    }

    // *********************************************************************************************
    //                           BIG ENDIAN READER STRUCT DEFINITIONS
    // *********************************************************************************************

    // Reads Unsigned 8-Bit Integers
    uint8_t Reader<BIGENDIAN>::ReadUInt8() {
        uint8_t value = file[position];
        position += sizeof(uint8_t);
        return value;
    }
    // Reads Unsigned 16-Bit Integers
    uint16_t Reader<BIGENDIAN>::ReadUInt16() {
        unsigned short typeSize = sizeof(uint16_t);
        unsigned char value[typeSize];
        memcpy(&value, &file[position], typeSize);
        position += typeSize;
        EndiannessConverter(Endianness::BIGENDIAN, endianness, value, typeSize);
        return static_cast<uint16_t>(CombineBytes(value, typeSize));
    }
    // Reads Unsigned 32-Bit Integers
    uint32_t Reader<BIGENDIAN>::ReadUInt32() {
        unsigned short typeSize = sizeof(uint32_t);
        unsigned char value[typeSize];
        memcpy(&value, &file[position], typeSize);
        position += typeSize;
        EndiannessConverter(Endianness::BIGENDIAN, endianness, value, typeSize);
        return static_cast<uint32_t>(CombineBytes(value, typeSize));
    }
    // Reads Unsigned 64-Bit Integers
    uint64_t Reader<BIGENDIAN>::ReadUInt64() {
        unsigned short typeSize = sizeof(uint64_t);
        unsigned char value[typeSize];
        memcpy(&value, &file[position], typeSize);
        position += typeSize;
        EndiannessConverter(Endianness::BIGENDIAN, endianness, value, typeSize);
        return static_cast<uint64_t>(CombineBytes(value, typeSize));
    }
    // Reads Signed 8-Bit Integers
    int8_t Reader<BIGENDIAN>::ReadInt8() {
        int8_t value = file[position];
        position += sizeof(int8_t);
        return value;
    }
    // Reads Signed 16-Bit Integers
    int16_t Reader<BIGENDIAN>::ReadInt16() {
        unsigned short typeSize = sizeof(int16_t);
        unsigned char value[typeSize];
        memcpy(&value, &file[position], typeSize);
        position += typeSize;
        EndiannessConverter(Endianness::BIGENDIAN, endianness, value, typeSize);
        return static_cast<int16_t>(CombineBytes(value, typeSize));
    }
    // Reads Signed 32-Bit Integers
    int32_t Reader<BIGENDIAN>::ReadInt32() {
        unsigned short typeSize = sizeof(int32_t);
        unsigned char value[typeSize];
        memcpy(&value, &file[position], typeSize);
        position += typeSize;
        EndiannessConverter(Endianness::BIGENDIAN, endianness, value, typeSize);
        return static_cast<int32_t>(CombineBytes(value, typeSize));
    }
    // Reads Signed 64-Bit Integers
    int64_t Reader<BIGENDIAN>::ReadInt64() {
        unsigned short typeSize = sizeof(int64_t);
        unsigned char value[typeSize];
        memcpy(&value, &file[position], typeSize);
        position += typeSize;
        EndiannessConverter(Endianness::BIGENDIAN, endianness, value, typeSize);
        return static_cast<int64_t>(CombineBytes(value, typeSize));
    }
    // Reads Character
    char Reader<BIGENDIAN>::ReadChar() {
        return static_cast<char>(ReadUInt8());
    }
    // Reads Signed Character
    signed char Reader<BIGENDIAN>::ReadSChar() {
        return static_cast<signed char>(ReadInt8());
    }
    // Reads Unsigned Character
    unsigned char Reader<BIGENDIAN>::ReadUChar() {
        return static_cast<unsigned char>(ReadUInt8());
    }
    // Reads Wide Character
    wchar_t Reader<BIGENDIAN>::ReadWChar() {
        unsigned short typeSize = sizeof(wchar_t);
        unsigned char value[typeSize];
        memcpy(&value, &file[position], typeSize);
        position += typeSize;
        EndiannessConverter(Endianness::BIGENDIAN, endianness, value, typeSize);
        return static_cast<wchar_t>(CombineBytes(value, typeSize));
    }
    // Reads 16-Bit Character
    char16_t Reader<BIGENDIAN>::ReadChar16() {
        unsigned short typeSize = sizeof(char16_t);
        unsigned char value[typeSize];
        memcpy(&value, &file[position], typeSize);
        position += typeSize;
        EndiannessConverter(Endianness::BIGENDIAN, endianness, value, typeSize);
        return static_cast<char16_t>(CombineBytes(value, typeSize));
    }
    // Reads 32-Bit Character
    char32_t Reader<BIGENDIAN>::ReadChar32() {
        unsigned short typeSize = sizeof(char32_t);
        unsigned char value[typeSize];
        memcpy(&value, &file[position], typeSize);
        position += typeSize;
        EndiannessConverter(Endianness::BIGENDIAN, endianness, value, typeSize);
        return static_cast<char32_t>(CombineBytes(value, typeSize));
    }
    // Reads String
    std::string Reader<BIGENDIAN>::ReadString(unsigned long int bytes) {
        std::string result(&file[position], bytes);
        position += bytes;
        return result;
    }
    // Reads Short
    short Reader<BIGENDIAN>::ReadShort() {
        return static_cast<short>(ReadInt16());
    }
    // Reads Unsigned Short
    unsigned short Reader<BIGENDIAN>::ReadUShort() {
        return static_cast<unsigned short>(ReadUInt16());
    }
    // Reads Integer
    int Reader<BIGENDIAN>::ReadInt() {
        return static_cast<int>(ReadInt32());
    }
    // Reads Unsigned Integer
    unsigned int Reader<BIGENDIAN>::ReadUInt() {
        return static_cast<unsigned int>(ReadUInt32());
    }
    // Reads Long
    long Reader<BIGENDIAN>::ReadLong() {
        unsigned short typeSize = sizeof(long);
        unsigned char value[typeSize];
        memcpy(&value, &file[position], typeSize);
        position += typeSize;
        EndiannessConverter(Endianness::BIGENDIAN, endianness, value, typeSize);
        return static_cast<long>(CombineBytes(value, typeSize));
    }
    // Reads Unsigned Long
    unsigned long Reader<BIGENDIAN>::ReadULong() {
        unsigned short typeSize = sizeof(unsigned long);
        unsigned char value[typeSize];
        memcpy(&value, &file[position], typeSize);
        position += typeSize;
        EndiannessConverter(Endianness::BIGENDIAN, endianness, value, typeSize);
        return static_cast<unsigned long>(CombineBytes(value, typeSize));
    }
    // Reads Long Long
    long long Reader<BIGENDIAN>::ReadLongLong() {
        unsigned short typeSize = sizeof(long long);
        unsigned char value[typeSize];
        memcpy(&value, &file[position], typeSize);
        position += typeSize;
        EndiannessConverter(Endianness::BIGENDIAN, endianness, value, typeSize);
        return static_cast<long long>(CombineBytes(value, typeSize));
    }
    // Reads Unsigned Long Long
    unsigned long long Reader<BIGENDIAN>::ReadULongLong() {
        unsigned short typeSize = sizeof(unsigned long long);
        unsigned char value[typeSize];
        memcpy(&value, &file[position], typeSize);
        position += typeSize;
        EndiannessConverter(Endianness::BIGENDIAN, endianness, value, typeSize);
        return static_cast<unsigned long long>(CombineBytes(value, typeSize));
    }
    // Reads Float
    float Reader<BIGENDIAN>::ReadFloat() {
        unsigned short typeSize = sizeof(float);
        unsigned char value[typeSize];
        memcpy(&value, &file[position], typeSize);
        position += typeSize;
        EndiannessConverter(Endianness::BIGENDIAN, endianness, value, typeSize);
        return static_cast<float>(CombineBytes(value, typeSize));
    }
    // Reads Double
    double Reader<BIGENDIAN>::ReadDouble() {
        unsigned short typeSize = sizeof(double);
        unsigned char value[typeSize];
        memcpy(&value, &file[position], typeSize);
        position += typeSize;
        EndiannessConverter(Endianness::BIGENDIAN, endianness, value, typeSize);
        return static_cast<double>(CombineBytes(value, typeSize));
    }
    // Reads Bool
    bool Reader<BIGENDIAN>::ReadBool() {
        return ReadUInt8() != 0;
    }
    // Reads Fixed Points And Interprets Them As Double
    double Reader<BIGENDIAN>::ReadFixedPoint(unsigned long int integerBits, unsigned long int fractionalBits) {
        unsigned long int totalBits = integerBits + fractionalBits;
        if ((totalBits % 8) != 0 || totalBits <= 0) {
            throw std::runtime_error("The sum of the number of integer and fractional bits must be a multiple of 8.");
        }
        unsigned long int totalBytes = (totalBits + 7) / 8;

        unsigned char value[totalBytes];
        memcpy(&value, &file[position], totalBytes);
        position += totalBytes;
        EndiannessConverter(Endianness::BIGENDIAN, endianness, value, totalBytes);
        uintmax_t data = CombineBytes(value, totalBytes);

        uintmax_t integerPart = data >> fractionalBits;
        uintmax_t fractionalPart = data & ((1ULL << fractionalBits) - 1);

        return (static_cast<double>(integerPart) + static_cast<double>(fractionalPart) / (1ULL << fractionalBits));
    }
}