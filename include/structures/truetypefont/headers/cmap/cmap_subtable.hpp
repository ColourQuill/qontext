#ifndef __TTF_CMAP_SUBTABLE_HPP__
#define __TTF_CMAP_SUBTABLE_HPP__

// qontext
#include <truetypefont/headers/cmap/cmap_formats.hpp>

namespace qtxt {
    struct Subtable {
        ~Subtable() {
            if (formatID != 255) {
                if (format != nullptr) {
                    delete format;
                }
            }
        }

        Subtable() {}

        Subtable(const Subtable& other)
            : platformID(other.platformID), platformSpecificID(other.platformSpecificID),
            offset(other.offset), formatID(other.formatID) {
            format = other.format ? other.format->clone() : nullptr;
        }

        Subtable& operator=(const Subtable& other) {
            if (this != &other) {
                platformID = other.platformID;
                platformSpecificID = other.platformSpecificID;
                offset = other.offset;
                formatID = other.formatID;
                
                delete format;
                format = other.format ? other.format->clone() : nullptr;
            }
            return *this;
        }

        uint16_t platformID; // 'Platform identifier' 
        // '(0 = Unicode, 1 = Macintosh, 2 = Reserved, 3 = Microsoft)'
        uint16_t platformSpecificID; // 'Platform-specific encoding identifier' 
        // Unicode Platform Specific IDs
        // 0 = 'Version 1.0 semantics'
        // 1 = 'Version 2.0 semantics'
        // 2 = 'ISO 10646 1993 semantics (Deprecated)'
        // 3 = 'Unicode 2.0 or later semantics (BMP only)'
        // 4 = 'Unicode 2.0 or later semantics (non-BMP characters allowed)'
        // 5 = 'Unicode Variation Sequences'
        // 6 = 'Last Resort'
        uint32_t offset; // 'Offset from the mapping table'
        
        uint8_t formatID{255};
        Format* format{nullptr};
    };
}

#endif