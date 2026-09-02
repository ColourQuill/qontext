#ifndef __QONTEXT_TTF_CMAP_FORMATS_HPP__
#define __QONTEXT_TTF_CMAP_FORMATS_HPP__

// std
#include <iostream>

// qontext
#include <info/type_defs.hpp>

namespace qtxt {
    struct Format {
        virtual ~Format() = default;
        uint8_t format;

        virtual Format* clone() const = 0;
    };

    struct Format0 : public Format {
        ~Format0() override {
            if (glyphIndexArray != nullptr) {
                delete[] glyphIndexArray;
            }
        }

        Format* clone() const override {
            Format0* copy = new Format0(*this);
            if (glyphIndexArraySize > 0) {
                copy->glyphIndexArray = new uint8_t[glyphIndexArraySize];
                std::copy(glyphIndexArray, glyphIndexArray + glyphIndexArraySize, copy->glyphIndexArray);
            }
            return copy;
        }

        uint16_t length;
        uint16_t language;
        uint8_t* glyphIndexArray{nullptr};
        uint32_t glyphIndexArraySize;
    };

    struct Format2 : public Format {
        ~Format2() override {
            if (subHeaderKeys != nullptr) {
                delete[] subHeaderKeys; 
            }
            if (subHeaders != nullptr) {
                delete[] subHeaders;
            }
            if (glyphIndexArray != nullptr) {
                delete[] glyphIndexArray;
            }
        }
        Format2* clone() const override {
            Format2* copy = new Format2(*this);
            if (subHeaderKeysSize > 0) {
                copy->subHeaderKeys = new uint16_t[subHeaderKeysSize];
                std::copy(subHeaderKeys, subHeaderKeys + subHeaderKeysSize, copy->subHeaderKeys);
            }
            if (glyphIndexArraySize > 0) {
                copy->glyphIndexArray = new uint16_t[glyphIndexArraySize];
                std::copy(glyphIndexArray, glyphIndexArray + glyphIndexArraySize, copy->glyphIndexArray);
            }
            return copy;
        }
        uint16_t length; // 'Total table length in bytes'
        uint16_t language; // 'Language code'
        uint16_t* subHeaderKeys{nullptr}; // 'Array that maps high bytes to subHeaders: value is index * 8'
        uint32_t subHeaderKeysSize;

        struct Subheader {
            uint16_t firstCode;
            uint16_t entryCount;
            int16_t idDelta;
            uint16_t idRangeOffset;
        };

        Subheader* subHeaders{nullptr}; // 'Variable length array of subHeader structures'
        uint32_t subHeadersSize;
        uint16_t* glyphIndexArray{nullptr}; // 'Variable length array containing subarrays'
        uint32_t glyphIndexArraySize;
    };

    struct Format4 : public Format {
        ~Format4() override {
            if (endCode != nullptr) {
                delete[] endCode;
            }
            if (startCode != nullptr) {
                delete[] startCode;
            }
            if (idDelta != nullptr) {
                delete[] idDelta;
            }
            if (idRangeOffset != nullptr) {
                delete[] idRangeOffset;
            }
            if (glyphIndexArray != nullptr) {
                delete[] glyphIndexArray;
            }
        }

        Format* clone() const override {
            Format4* copy = new Format4(*this);
            if (segCount > 0) {
                copy->endCode = new uint16_t[segCount];
                std::copy(endCode, endCode + segCount, copy->endCode);
                copy->startCode = new uint16_t[segCount];
                std::copy(startCode, startCode + segCount, copy->startCode);
                copy->idDelta = new uint16_t[segCount];
                std::copy(idDelta, idDelta + segCount, copy->idDelta);
                copy->idRangeOffset = new uint16_t[segCount];
                std::copy(idRangeOffset, idRangeOffset + segCount, copy->idRangeOffset);
            }
            if (glyphIndexArray) {
                // Calculate the actual number of elements in glyphIndexArray
                size_t glyphIndexArraySize = (copy->length - 16 - 4 * copy->segCountX2) / 2;
                copy->glyphIndexArray = new uint16_t[glyphIndexArraySize];
                std::copy(glyphIndexArray, glyphIndexArray + glyphIndexArraySize, copy->glyphIndexArray);
            }
            return copy;
        }

        uint16_t length; // 'Length of subtable in bytes'
        uint16_t language; // 'Language code'
        uint16_t segCountX2; // '2 * segCount'
        uint16_t segCount;
        uint16_t searchRange; // '2 * (2**FLOOR(log_2(segCount)))'
        uint16_t entrySelector; // 'log_2(searchRange/2)'
        uint16_t rangeShift; // '(2 * segCount) - searchRange'
        uint16_t* endCode{nullptr}; // 'Ending character code for each segment, last = 0xFFFF'
        uint16_t reservedPad; // 'This value should be zero'
        uint16_t* startCode{nullptr}; // 'Start character code for each segment'
        uint16_t* idDelta{nullptr}; // 'Delta for all character codes in segment'
        uint16_t* idRangeOffset{nullptr}; // 'Offset in bytes to glyph indexArray, or 0'
        uint16_t* glyphIndexArray{nullptr}; // 'Glyph index array'
    };

    struct Format6 : public Format {
        ~Format6() override {
            if (glyphIndexArray != nullptr) {
                delete[] glyphIndexArray;
            }
        }

        Format* clone() const override {
            Format6* copy = new Format6(*this);
            if (entryCount > 0) {
                copy->glyphIndexArray = new uint16_t[entryCount];
                std::copy(glyphIndexArray, glyphIndexArray + entryCount, copy->glyphIndexArray);
            }
            return copy;
        }

        uint16_t length; // 'Length in bytes'
        uint16_t language; // 'Language code'
        uint16_t firstCode; // 'First character code of subrange'
        uint16_t entryCount; // 'Number of character codes in subrange'
        uint16_t* glyphIndexArray{nullptr}; // 'Array of glyph index values for character codes in the range'
    };

    struct Format8 : public Format {
        ~Format8() override {
            if (is32 != nullptr) {
                delete[] is32;
            }
            if (groups != nullptr) {
                delete[] groups;
            }
        }

        Format* clone() const override {
            Format8* copy = new Format8(*this);
            if (is32Size > 0) {
                copy->is32 = new uint8_t[is32Size];
                std::copy(is32, is32 + is32Size, copy->is32);
            }
            if (nGroups > 0) {
                copy->groups = new Group[nGroups];
                std::copy(groups, groups + nGroups, copy->groups);
            }
            return copy;
        }

        uint16_t reserved; // 'Set to 0'
        uint32_t length; // 'Byte length of this subtable (including the header)'
        uint32_t langugage; // 'Language code'
        uint8_t* is32{nullptr}; // 'Tightly packed array of bits (8K bytes total) indicating whether the particular 16-bit (index) value is the start of the 32-bit character code'
        uint32_t is32Size;
        uint32_t nGroups; // 'Number of groups which follow'

        struct Group {
            uint32_t startCharCode; // 'First character code in this group; note that if this group is one or more 16-bit character code, this 32-bit value will have the high 16-bits set to 0'
            uint32_t endCharCode; // 'Last character code in this group; same condition as listed above for startCharCode'
            uint32_t startGlyphCode; // 'Glyph index corresponding to the starting character code'
        };

        Group* groups{nullptr};
    };

    struct Format10 : public Format {
        ~Format10() override {
            if (glyphs != nullptr) {
                delete[] glyphs;
            }
        }

        Format* clone() const override {
            Format10* copy = new Format10(*this);
            if (numChars > 0) {
                copy->glyphs = new uint16_t[numChars];
                std::copy(glyphs, glyphs + numChars, copy->glyphs);
            }
            return copy;
        }

        uint16_t reserved; // 'Set to 10'
        uint32_t length; // 'Byte length of this subtable (including the header)'
        uint32_t language; // 'Language code'
        uint32_t startCharCode; // 'First character code covered'
        uint32_t numChars; // 'Number of character codes covered'
        uint16_t* glyphs{nullptr}; // 'Array of glyph indices for the character codes covered'
    };

    struct Format12 : public Format {
        ~Format12() override {
            if (groups != nullptr) {
                delete[] groups;
            }
        }

        Format* clone() const override {
            Format12* copy = new Format12(*this);
            if (nGroups > 0) {
                copy->groups = new Group[nGroups];
                std::copy(groups, groups + nGroups, copy->groups);
            }
            return copy;
        }

        uint16_t reserved; // 'Set to 0'
        uint32_t length; // 'Byte length of this subtable (including the header)'
        uint32_t language; // 'Language code'
        uint32_t nGroups; // 'Number of groups which follow'

        struct Group {
            uint32_t startCharCode; // 'First character code in this group'
            uint32_t endCharCode; // 'Last character code in this group'
            uint32_t startGlyphCode; // 'Glyph index corresponding to the starting character code; subsequent characters are mapped to sequential glyphs'
        };

        Group* groups{nullptr};
    };

    struct Format13 : public Format {
        ~Format13() override {
            if (groups != nullptr) {
                delete[] groups;
            }
        }

        Format* clone() const override {
            Format13* copy = new Format13(*this);
            if (nGroups > 0) {
                copy->groups = new Group[nGroups];
                std::copy(groups, groups + nGroups, copy->groups);
            }
            return copy;
        }

        uint16_t reserved; // 'Set to 0'
        uint32_t length; // 'Byte length of this subtable (including the header)'
        uint32_t language; // 'Language code'
        uint32_t nGroups; // 'Number of groups which follow'

        struct Group {
            uint32_t startCharCode; // 'First character code in this group'
            uint32_t endCharCode; // 'Last character code in this group'
            uint32_t startGlyphCode; // 'Glyph index for this group'
        };

        Group* groups{nullptr};
    };

    struct Format14 : public Format {
        ~Format14() override {
            if (variationSelectorRecords != nullptr) {
                delete[] variationSelectorRecords;
            }
        }

        Format* clone() const override {
            Format14* copy = new Format14(*this);
            if (numVarSelectorRecords > 0) {
                copy->variationSelectorRecords = new VariationSelectorRecord[numVarSelectorRecords];
                std::copy(variationSelectorRecords, variationSelectorRecords + numVarSelectorRecords, copy->variationSelectorRecords);
            }
            if (defaultUVSTable.numUnicodeValueRanges > 0) {
                copy->defaultUVSTable.unicodeValueRanges = new DefaultUVsTable::UnicodeValueRange[defaultUVSTable.numUnicodeValueRanges];
                std::copy(defaultUVSTable.unicodeValueRanges, defaultUVSTable.unicodeValueRanges + defaultUVSTable.numUnicodeValueRanges, copy->defaultUVSTable.unicodeValueRanges);
            }
            if (nonDefaultUVSTable.numUVSMappings > 0) {
                copy->nonDefaultUVSTable.uvsMappings = new NonDefaultUVsTable::UVsMapping[nonDefaultUVSTable.numUVSMappings];
                std::copy(nonDefaultUVSTable.uvsMappings, nonDefaultUVSTable.uvsMappings + nonDefaultUVSTable.numUVSMappings, copy->nonDefaultUVSTable.uvsMappings);
            }
            return copy;
        }

        uint32_t length; // 'Byte length of this subtable (including the header)'
        uint32_t numVarSelectorRecords; // 'Number of variation Selector Records'

        struct VariationSelectorRecord {
            uint32_t varSelector; // 'Variation Selector' (Is stored in the file as a uint24 but that is rarely support so in this lib is a uint32_t)
            uint32_t defaultUVSOffset; // 'Offset to Default UVS Table. May be 0.'
            uint32_t nonDefaultUVSOffset; // 'Offset to Non-Default UVS Table. May be 0.'
        };

        VariationSelectorRecord* variationSelectorRecords{nullptr};

        struct DefaultUVsTable {
            ~DefaultUVsTable() {
                if (unicodeValueRanges != nullptr) {
                    delete[] unicodeValueRanges;
                }
            }
            uint32_t numUnicodeValueRanges; // 'Number of ranges that follow'
            
            struct UnicodeValueRange {
                uint32_t startUnicodeValue; // 'First value in this range' (Stored as uint24 in file but is stored as uint32_t in this lib for supportability)
                uint8_t additionalCount; // 'Number of additional values in this range' 
            };

            UnicodeValueRange* unicodeValueRanges{nullptr};
        };

        DefaultUVsTable defaultUVSTable;

        struct NonDefaultUVsTable {
            ~NonDefaultUVsTable() {
                if (uvsMappings != nullptr) {
                    delete[] uvsMappings;
                }
            }

            uint32_t numUVSMappings; // 'Number of UVS Mappings that follow'
            
            struct UVsMapping {
                uint32_t unicodeValue; // 'Base Unicode value of the UVS' (Stored as uint24 in file but is stored as uint32_t in this lib for supportability)
                uint16_t glyphID; // 'Glyph ID of the UVS'
            };

            UVsMapping* uvsMappings{nullptr};
        };

        NonDefaultUVsTable nonDefaultUVSTable;
    };
}

#endif