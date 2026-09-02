#ifndef __QONTEXT_TTF_NAME_HPP__
#define __QONTEXT_TTF_NAME_HPP__

// std
#include <string>

// qontext
#include <info/type_defs.hpp>

namespace qtxt {
    struct NAME { // 'naming'
        ~NAME() {
            if (nameRecords != nullptr) {
                delete[] nameRecords;
            }
        }
        struct NameRecord {
            uint16_t platformID; // 'Platform identifier code'
            uint16_t platformSpecificID; // 'Platform-specific encoding identifier'
            uint16_t languageID; // 'Language identifier'
            uint16_t nameID; // 'Name identifier'
            uint16_t length; // 'Name string length in bytes'
            uint16_t offset; // 'Name string offset in bytes from stringOffset'
            std::string name;
        };

        uint32_t offset;
        uint16_t format; // 'Format selector. Set to 0.'
        uint16_t count; // 'The number of nameRecords in this name table.'
        uint16_t stringOffset; // 'Offset in bytes to the beginning of the name character strings.'
        NameRecord* nameRecords{nullptr};
    };
}

#endif