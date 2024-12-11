#include <writer/writer.hpp>

// std
#include <iostream>

namespace qtxt {
    // *********************************************************************************************
    //                              BINARY WRITER STRUCT DEFINITIONS
    // *********************************************************************************************
    void Writer<BINARY>::OpenFile(const std::string& path) {
        OpenFile(path.c_str());
    }
    void Writer<BINARY>::OpenFile(const char* path) {
        file.open(path, std::ios::binary | std::ios::out);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open binary file for writing");
        }
        position = 0;

        endianness = GetEndianness();
    }
    void Writer<BINARY>::CloseFile() {
        if (file.is_open()) {
            file.close();
        }
    }
    void Writer<BINARY>::ClearBuffer() {
        file.flush();
        position = 0;
    }
    void Writer<BINARY>::WriteBytes(const char* data, unsigned long long size) {
        if (!file.is_open()) {
            throw std::runtime_error("File is not open for writing");
        }
        file.write(data, size);
        position += size;
    }
    void Writer<BINARY>::WriteByte(const char byte) {
        WriteBytes(&byte, 1);
    }

    // *********************************************************************************************
    //                                TEXT WRITER STRUCT DEFINITIONS
    // *********************************************************************************************

    void Writer<TEXT>::OpenFile(const std::string& path) {
        file.open(path, std::ios::in | std::ios::out | std::ios::app);

        if (!file.is_open()) {
            file.open(path, std::ios::out);
            file.close();
            file.open(path, std::ios::in | std::ios::out);
        }

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open or create file");
        }

        file.clear();
        file.seekg(0, std::ios::beg);
        std::string line;
        while (std::getline(file, line)) {
            buffer.push_back(line);
        }

        position = 0;
    }
    void Writer<TEXT>::OpenFile(const char* path) {
        OpenFile(std::string(path));
    }
    void Writer<TEXT>::SaveFile(const std::string& path) {
        CloseFile();
        file.open(path, std::ios::out | std::ios::trunc);
        if (!file.is_open()) {
            throw std::runtime_error("File is not open for writing");
        }
        for (auto line : buffer) {
            file << line << '\n';
            position++;
        }
        CloseFile();
    }
    void Writer<TEXT>::SaveFile(const char* path) {
        SaveFile(std::string(path));
    }
    void Writer<TEXT>::CloseFile() {
        if (file.is_open()) {
            file.close();
        }
    }
    void Writer<TEXT>::ClearBuffer() {
        buffer.clear();
        position = 0;
    }
    void Writer<TEXT>::WriteLine(const std::string& line, long long lineIndex) {
        if (lineIndex >= buffer.size() || lineIndex < 0) {
            buffer.push_back(line);
        } else {
            buffer[lineIndex] = line;
        }
    }
    void Writer<TEXT>::WriteLines(const std::vector<std::string>& lines) {
        for (const auto& line : lines) {
            WriteLine(line);
        }
    }
    

    // *********************************************************************************************
    //                            BIG ENDIAN WRITER STRUCT DEFINITIONS
    // *********************************************************************************************

    // Writes Unsigned 8-Bit Integers
    void Writer<BIGENDIAN>::WriteUInt8(uint8_t value) {
        WriteByte(static_cast<char>(value));
    }
    // Writes Unsigned 16-Bit Integers
    void Writer<BIGENDIAN>::WriteUInt16(uint16_t value) {
        unsigned short typeSize = sizeof(uint16_t);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::BIGENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Unsigned 32-Bit Integers
    void Writer<BIGENDIAN>::WriteUInt32(uint32_t value) {
        unsigned short typeSize = sizeof(uint32_t);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::BIGENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Unsigned 64-Bit Integers
    void Writer<BIGENDIAN>::WriteUInt64(uint64_t value) {
        unsigned short typeSize = sizeof(uint64_t);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::BIGENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Signed 8-Bit Integers
    void Writer<BIGENDIAN>::WriteInt8(int8_t value) {
        WriteByte(static_cast<char>(value));
    }
    // Writes Signed 16-Bit Integers
    void Writer<BIGENDIAN>::WriteInt16(int16_t value) {
        unsigned short typeSize = sizeof(int16_t);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::BIGENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Signed 32-Bit Integers
    void Writer<BIGENDIAN>::WriteInt32(int32_t value) {
        unsigned short typeSize = sizeof(int32_t);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::BIGENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Signed 64-Bit Integers
    void Writer<BIGENDIAN>::WriteInt64(int64_t value) {
        unsigned short typeSize = sizeof(int64_t);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::BIGENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Character
    void Writer<BIGENDIAN>::WriteChar(char value) {
        WriteUInt8((uint8_t)value);
    }
    // Writes String
    void Writer<BIGENDIAN>::WriteString(const std::string& value) {
        WriteBytes(value.data(), value.size());
    }
    // Writes Short
    void Writer<BIGENDIAN>::WriteShort(short value) {
        unsigned short typeSize = sizeof(short);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::BIGENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Unsigned Short
    void Writer<BIGENDIAN>::WriteUShort(unsigned short value) {
        unsigned short typeSize = sizeof(unsigned short);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::BIGENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Int
    void Writer<BIGENDIAN>::WriteInt(int value) {
        unsigned short typeSize = sizeof(int);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::BIGENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Unsigned Int
    void Writer<BIGENDIAN>::WriteUInt(unsigned int value) {
        unsigned short typeSize = sizeof(unsigned int);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::BIGENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Long
    void Writer<BIGENDIAN>::WriteLong(long value) {
        unsigned short typeSize = sizeof(long);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::BIGENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Unsigned Long
    void Writer<BIGENDIAN>::WriteULong(unsigned long value) {
        unsigned short typeSize = sizeof(unsigned long);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::BIGENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Long Long
    void Writer<BIGENDIAN>::WriteLongLong(long long value) {
        unsigned short typeSize = sizeof(long long);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::BIGENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Unsigned Long Long
    void Writer<BIGENDIAN>::WriteULongLong(unsigned long long value) {
        unsigned short typeSize = sizeof(unsigned long long);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::BIGENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Float
    void Writer<BIGENDIAN>::WriteFloat(float value) {
        unsigned short typeSize = sizeof(float);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::BIGENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Double
    void Writer<BIGENDIAN>::WriteDouble(double value) {
        unsigned short typeSize = sizeof(double);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::BIGENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Bool
    void Writer<BIGENDIAN>::WriteBool(bool value) {
        WriteByte((char)value);
    }
    // Writes Fixed Point
    void Writer<BIGENDIAN>::WriteFixedPoint(double value, unsigned long int integerBits, unsigned long int fractionalBits) {
        unsigned long int totalBits = integerBits + fractionalBits;
        if ((totalBits & 8) != 0 || totalBits <= 0) {
            throw std::runtime_error("The sum of the number of integer and fractional bits must be a multiple of 8.");
        }
        unsigned long int totalBytes = (totalBits + 7) / 8;

        uintmax_t integerPart = static_cast<uintmax_t>(value);
        uintmax_t fractionalPart = static_cast<uintmax_t>((value - static_cast<double>(integerPart)) * (1ULL << fractionalBits));

        uintmax_t data = (integerPart << fractionalBits) | (fractionalPart & ((1ULL << fractionalBits) - 1));

        unsigned char bytes[totalBytes];
        SeparateBytes(data, bytes, totalBytes);

        EndiannessConverter(endianness, Endianness::BIGENDIAN, bytes, totalBytes);

        WriteBytes(reinterpret_cast<const char*>(bytes), totalBytes);
    }

    // *********************************************************************************************
    //                            LITTLE ENDIAN WRITER STRUCT DEFINITIONS
    // *********************************************************************************************

    // Writes Unsigned 8-Bit Integers
    void Writer<LITTLEENDIAN>::WriteUInt8(uint8_t value) {
        WriteByte(static_cast<char>(value));
    }
    // Writes Unsigned 16-Bit Integers
    void Writer<LITTLEENDIAN>::WriteUInt16(uint16_t value) {
        unsigned short typeSize = sizeof(uint16_t);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::LITTLEENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Unsigned 32-Bit Integers
    void Writer<LITTLEENDIAN>::WriteUInt32(uint32_t value) {
        unsigned short typeSize = sizeof(uint32_t);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::LITTLEENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Unsigned 64-Bit Integers
    void Writer<LITTLEENDIAN>::WriteUInt64(uint64_t value) {
        unsigned short typeSize = sizeof(uint64_t);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::LITTLEENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Signed 8-Bit Integers
    void Writer<LITTLEENDIAN>::WriteInt8(int8_t value) {
        WriteByte(static_cast<char>(value));
    }
    // Writes Signed 16-Bit Integers
    void Writer<LITTLEENDIAN>::WriteInt16(int16_t value) {
        unsigned short typeSize = sizeof(int16_t);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::LITTLEENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Signed 32-Bit Integers
    void Writer<LITTLEENDIAN>::WriteInt32(int32_t value) {
        unsigned short typeSize = sizeof(int32_t);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::LITTLEENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Signed 64-Bit Integers
    void Writer<LITTLEENDIAN>::WriteInt64(int64_t value) {
        unsigned short typeSize = sizeof(int64_t);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::LITTLEENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Character
    void Writer<LITTLEENDIAN>::WriteChar(char value) {
        WriteUInt8((uint8_t)value);
    }
    // Writes String
    void Writer<LITTLEENDIAN>::WriteString(const std::string& value) {
        WriteBytes(value.data(), value.size());
    }
    // Writes Short
    void Writer<LITTLEENDIAN>::WriteShort(short value) {
        unsigned short typeSize = sizeof(short);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::LITTLEENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Unsigned Short
    void Writer<LITTLEENDIAN>::WriteUShort(unsigned short value) {
        unsigned short typeSize = sizeof(unsigned short);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::LITTLEENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Int
    void Writer<LITTLEENDIAN>::WriteInt(int value) {
        unsigned short typeSize = sizeof(int);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::LITTLEENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Unsigned Int
    void Writer<LITTLEENDIAN>::WriteUInt(unsigned int value) {
        unsigned short typeSize = sizeof(unsigned int);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::LITTLEENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Long
    void Writer<LITTLEENDIAN>::WriteLong(long value) {
        unsigned short typeSize = sizeof(long);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::LITTLEENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Unsigned Long
    void Writer<LITTLEENDIAN>::WriteULong(unsigned long value) {
        unsigned short typeSize = sizeof(unsigned long);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::LITTLEENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Long Long
    void Writer<LITTLEENDIAN>::WriteLongLong(long long value) {
        unsigned short typeSize = sizeof(long long);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::LITTLEENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Unsigned Long Long
    void Writer<LITTLEENDIAN>::WriteULongLong(unsigned long long value) {
        unsigned short typeSize = sizeof(unsigned long long);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::LITTLEENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Float
    void Writer<LITTLEENDIAN>::WriteFloat(float value) {
        unsigned short typeSize = sizeof(float);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::LITTLEENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Double
    void Writer<LITTLEENDIAN>::WriteDouble(double value) {
        unsigned short typeSize = sizeof(double);
        unsigned char bytes[typeSize];
        SeparateBytes(value, bytes, typeSize);
        EndiannessConverter(endianness, Endianness::LITTLEENDIAN, bytes, typeSize);
        WriteBytes(reinterpret_cast<const char*>(bytes), typeSize);
    }
    // Writes Bool
    void Writer<LITTLEENDIAN>::WriteBool(bool value) {
        WriteByte((char)value);
    }
    // Writes Fixed Point
    void Writer<LITTLEENDIAN>::WriteFixedPoint(double value, unsigned long int integerBits, unsigned long int fractionalBits) {
        unsigned long int totalBits = integerBits + fractionalBits;
        if ((totalBits & 8) != 0 || totalBits <= 0) {
            throw std::runtime_error("The sum of the number of integer and fractional bits must be a multiple of 8.");
        }
        unsigned long int totalBytes = (totalBits + 7) / 8;

        uintmax_t integerPart = static_cast<uintmax_t>(value);
        uintmax_t fractionalPart = static_cast<uintmax_t>((value - static_cast<double>(integerPart)) * (1ULL << fractionalBits));

        uintmax_t data = (integerPart << fractionalBits) | (fractionalPart & ((1ULL << fractionalBits) - 1));

        unsigned char bytes[totalBytes];
        SeparateBytes(data, bytes, totalBytes);

        EndiannessConverter(endianness, Endianness::LITTLEENDIAN, bytes, totalBytes);

        WriteBytes(reinterpret_cast<const char*>(bytes), totalBytes);
    }
}