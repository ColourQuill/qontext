#ifndef __QONTEXT_TRUETYPEFONT_HPP__
#define __QONTEXT_TRUETYPEFONT_HPP__

// *********************************************************************************************
// https://developer.apple.com/fonts/TrueType-Reference-Manual/
// *********************************************************************************************

// Struct is based on the given documentation above

// std
#include <cstdint>
#include <iostream>

// qarbon
#include <qarbon.hpp>

namespace qtxt {
    using shortFrac = uint16_t;
    using Fixed = int32_t;
    using FWord = int16_t;
    using uFWord = uint16_t;
    using longDateTime = int64_t;

    struct StateHeader {
        uint16_t stateSize; // 'Size of a state, in bytes. The size is limited to 8 bits, although the field is 16 bits for alignment.'
        uint16_t classTable; // 'Byte offset from the beginning of the state table to the class subtable.'
        uint16_t stateArray; // 'Byte offset from the beginning of the state table to the state array.'
        uint16_t entryTable; // 'Byte offset from the beginning of the state table to the entry subtable.'
    };

    struct CMAP { // 'character to glyph mapping'
        ~CMAP() {
            if (subtables != nullptr) {
                delete[] subtables;
            }
        }
        struct FORMAT {
            virtual ~FORMAT() = default;
            uint8_t format;

            virtual FORMAT* clone() const = 0;
        };
        struct FORMAT0 : public FORMAT {
            ~FORMAT0() override { 
                if (glyphIndexArray != nullptr) {
                    delete[] glyphIndexArray; 
                }
            }
            FORMAT* clone() const override {
                FORMAT0* copy = new FORMAT0(*this);  // Use copy constructor
                if (glyphIndexArraySize > 0) {
                    copy->glyphIndexArray = new uint8_t[glyphIndexArraySize];
                    std::copy(glyphIndexArray, glyphIndexArray + glyphIndexArraySize, copy->glyphIndexArray);  // Deep copy of glyphIndexArray
                }
                return copy;
            }
            uint16_t length; // 'Length in bytes of the subtable (set to 262 for format 0)'
            uint16_t language; // 'Language code'
            uint8_t* glyphIndexArray{nullptr}; // 'An array that maps character codes to glyph index values'
            uint32_t glyphIndexArraySize;
        };
        struct FORMAT2 : public FORMAT {
            ~FORMAT2() override {
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
            FORMAT* clone() const override {
                FORMAT2* copy = new FORMAT2(*this);  // Use copy constructor
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

            struct SUBHEADER {
                uint16_t firstCode;
                uint16_t entryCount;
                int16_t idDelta;
                uint16_t idRangeOffset;
            };

            SUBHEADER* subHeaders{nullptr}; // 'Variable length array of subHeader structures'
            uint32_t subHeadersSize;
            uint16_t* glyphIndexArray{nullptr}; // 'Variable length array containing subarrays'
            uint32_t glyphIndexArraySize;
        };
        struct FORMAT4 : public FORMAT {
            ~FORMAT4() override {
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

            FORMAT* clone() const override {
                FORMAT4* copy = new FORMAT4(*this);
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
        struct FORMAT6 : public FORMAT {
            ~FORMAT6() override {
                if (glyphIndexArray != nullptr) {
                    delete[] glyphIndexArray;
                }
            }

            FORMAT* clone() const override {
                FORMAT6* copy = new FORMAT6(*this);
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
        struct FORMAT8 : public FORMAT {
            ~FORMAT8() override {
                if (is32 != nullptr) {
                    delete[] is32;
                }
                if (groups != nullptr) {
                    delete[] groups;
                }
            }

            FORMAT* clone() const override {
                FORMAT8* copy = new FORMAT8(*this);
                if (is32Size > 0) {
                    copy->is32 = new uint8_t[is32Size];
                    std::copy(is32, is32 + is32Size, copy->is32);
                }
                if (nGroups > 0) {
                    copy->groups = new GROUP[nGroups];
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

            struct GROUP {
                uint32_t startCharCode; // 'First character code in this group; note that if this group is one or more 16-bit character code, this 32-bit value will have the high 16-bits set to 0'
                uint32_t endCharCode; // 'Last character code in this group; same condition as listed above for startCharCode'
                uint32_t startGlyphCode; // 'Glyph index corresponding to the starting character code'
            };

            GROUP* groups{nullptr};
        };
        struct FORMAT10 : public FORMAT {
            ~FORMAT10() override {
                if (glyphs != nullptr) {
                    delete[] glyphs;
                }
            }

            FORMAT* clone() const override {
                FORMAT10* copy = new FORMAT10(*this);
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
        struct FORMAT12 : public FORMAT {
            ~FORMAT12() override {
                if (groups != nullptr) {
                    delete[] groups;
                }
            }

            FORMAT* clone() const override {
                FORMAT12* copy = new FORMAT12(*this);
                if (nGroups > 0) {
                    copy->groups = new GROUP[nGroups];
                    std::copy(groups, groups + nGroups, copy->groups);
                }
                return copy;
            }

            uint16_t reserved; // 'Set to 0'
            uint32_t length; // 'Byte length of this subtable (including the header)'
            uint32_t language; // 'Language code'
            uint32_t nGroups; // 'Number of groups which follow'

            struct GROUP {
                uint32_t startCharCode; // 'First character code in this group'
                uint32_t endCharCode; // 'Last character code in this group'
                uint32_t startGlyphCode; // 'Glyph index corresponding to the starting character code; subsequent characters are mapped to sequential glyphs'
            };

            GROUP* groups{nullptr};
        };
        struct FORMAT13 : public FORMAT {
            ~FORMAT13() override {
                if (groups != nullptr) {
                    delete[] groups;
                }
            }

            FORMAT* clone() const override {
                FORMAT13* copy = new FORMAT13(*this);
                if (nGroups > 0) {
                    copy->groups = new GROUP[nGroups];
                    std::copy(groups, groups + nGroups, copy->groups);
                }
                return copy;
            }

            uint16_t reserved; // 'Set to 0'
            uint32_t length; // 'Byte length of this subtable (including the header)'
            uint32_t language; // 'Language code'
            uint32_t nGroups; // 'Number of groups which follow'

            struct GROUP {
                uint32_t startCharCode; // 'First character code in this group'
                uint32_t endCharCode; // 'Last character code in this group'
                uint32_t startGlyphCode; // 'Glyph index for this group'
            };

            GROUP* groups{nullptr};
        };
        struct FORMAT14 : public FORMAT {
            ~FORMAT14() override {
                if (variationSelectorRecords != nullptr) {
                    delete[] variationSelectorRecords;
                }
            }

            FORMAT* clone() const override {
                FORMAT14* copy = new FORMAT14(*this);
                if (numVarSelectorRecords > 0) {
                    copy->variationSelectorRecords = new VARIATIONSELECTORRECORD[numVarSelectorRecords];
                    std::copy(variationSelectorRecords, variationSelectorRecords + numVarSelectorRecords, copy->variationSelectorRecords);
                }
                if (defaultUVSTable.numUnicodeValueRanges > 0) {
                    copy->defaultUVSTable.unicodeValueRanges = new DEFAULTUVSTABLE::UNICODEVALUERANGE[defaultUVSTable.numUnicodeValueRanges];
                    std::copy(defaultUVSTable.unicodeValueRanges, defaultUVSTable.unicodeValueRanges + defaultUVSTable.numUnicodeValueRanges, copy->defaultUVSTable.unicodeValueRanges);
                }
                if (nonDefaultUVSTable.numUVSMappings > 0) {
                    copy->nonDefaultUVSTable.uvsMappings = new NONDEFAULTUVSTABLE::UVSMAPPING[nonDefaultUVSTable.numUVSMappings];
                    std::copy(nonDefaultUVSTable.uvsMappings, nonDefaultUVSTable.uvsMappings + nonDefaultUVSTable.numUVSMappings, copy->nonDefaultUVSTable.uvsMappings);
                }
                return copy;
            }

            uint32_t length; // 'Byte length of this subtable (including the header)'
            uint32_t numVarSelectorRecords; // 'Number of variation Selector Records'

            struct VARIATIONSELECTORRECORD {
                uint32_t varSelector; // 'Variation Selector' (Is stored in the file as a uint24 but that is rarely support so in this lib is a uint32_t)
                uint32_t defaultUVSOffset; // 'Offset to Default UVS Table. May be 0.'
                uint32_t nonDefaultUVSOffset; // 'Offset to Non-Default UVS Table. May be 0.'
            };

            VARIATIONSELECTORRECORD* variationSelectorRecords{nullptr};

            struct DEFAULTUVSTABLE {
                ~DEFAULTUVSTABLE() {
                    if (unicodeValueRanges != nullptr) {
                        delete[] unicodeValueRanges;
                    }
                }
                uint32_t numUnicodeValueRanges; // 'Number of ranges that follow'
                
                struct UNICODEVALUERANGE {
                    uint32_t startUnicodeValue; // 'First value in this range' (Stored as uint24 in file but is stored as uint32_t in this lib for supportability)
                    uint8_t additionalCount; // 'Number of additional values in this range' 
                };

                UNICODEVALUERANGE* unicodeValueRanges{nullptr};
            };

            DEFAULTUVSTABLE defaultUVSTable;

            struct NONDEFAULTUVSTABLE {
                ~NONDEFAULTUVSTABLE() {
                    if (uvsMappings != nullptr) {
                        delete[] uvsMappings;
                    }
                }

                uint32_t numUVSMappings; // 'Number of UVS Mappings that follow'
                
                struct UVSMAPPING {
                    uint32_t unicodeValue; // 'Base Unicode value of the UVS' (Stored as uint24 in file but is stored as uint32_t in this lib for supportability)
                    uint16_t glyphID; // 'Glyph ID of the UVS'
                };

                UVSMAPPING* uvsMappings{nullptr};
            };

            NONDEFAULTUVSTABLE nonDefaultUVSTable;
        };

        struct SUBTABLE {
            ~SUBTABLE() {
                if (formatID != 255) {
                    if (format != nullptr) {
                        delete format;
                    }
                }
            }

            SUBTABLE() {}

            // Copy constructor for deep copy
            SUBTABLE(const SUBTABLE& other)
                : platformID(other.platformID), platformSpecificID(other.platformSpecificID),
                offset(other.offset), formatID(other.formatID) {
                format = other.format ? other.format->clone() : nullptr;  // Deep copy of format
            }

            // Copy assignment operator for deep copy
            SUBTABLE& operator=(const SUBTABLE& other) {
                if (this != &other) {
                    platformID = other.platformID;
                    platformSpecificID = other.platformSpecificID;
                    offset = other.offset;
                    formatID = other.formatID;
                    
                    delete format;  // Clean up old format object
                    format = other.format ? other.format->clone() : nullptr;  // Deep copy of format
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
            FORMAT* format{nullptr};
        };

        uint32_t offset;
        uint16_t version; // 'Version number (set to zero)'
        uint16_t numberSubtables; // 'Number of encoding subtables'
        SUBTABLE* subtables{nullptr};
    };
    struct GLYF { // 'glyph data'
        ~GLYF() {
            for (int i = 0; i < numGlyphs; ++i) {
                if (glyphs[i] != nullptr) {
                    delete glyphs[i]; // Delete each glyph
                    glyphs[i] = nullptr; // Optional: prevent dangling pointers
                }
            }
            delete[] glyphs; // Delete the array of pointers
            glyphs = nullptr; // Optional: prevent dangling pointers
        }
        struct GLYPH {
            virtual ~GLYPH() = default;
            uint8_t glyphType;
        };
        struct SIMPLEGLYPH : public GLYPH {
            ~SIMPLEGLYPH() {
                if (endPtsOfContours != nullptr) {
                    delete[] endPtsOfContours;
                }
                if (instructions != nullptr) {
                    delete[] instructions;
                }
                if (flags != nullptr) {
                    delete[] flags;
                }
                if (points != nullptr) {
                    delete[] points;
                }
            }
            int16_t numberOfContours; // 'If the number of contours is positive or zero, it is a simple glyph; If the number of contours less than zero, the glyph is compound'
            qbn::vec<FWord, 2> min; // Minimum coordinate data
            qbn::vec<FWord, 2> max; // Maximum coordinate data
            uint16_t* endPtsOfContours{nullptr}; // 'Array of last points of each contour; n is the number of contours; array entries are point indices'
            uint16_t instructionLength; // 'Total number of bytes needed for instructions'
            uint8_t* instructions{nullptr}; // 'Array of instructions for this glyph'
            uint8_t* flags{nullptr};
            uint32_t numFlags;

            struct POINT { // Isn't exactly what the documentation has the for coordinates, but its more useful in this form 99% of the time imo
                qbn::vec<int16_t, 2> coordinates;
                bool onCurve;
            };

            POINT* points{nullptr};
        };
        struct COMPOUNDGLYPH : public GLYPH {
            ~COMPOUNDGLYPH() {
                if (component_glyphs != nullptr) {
                    delete[] component_glyphs;
                }
            }
            struct COMPONENTGLYPH {
                ~COMPONENTGLYPH() {
                    if (transform != nullptr) {
                        delete transform;
                    }
                }

                uint16_t flags;
                uint16_t glyphIndex;
                qbn::vec2<int16_t> offset;

                struct TRANSFORM {
                    virtual ~TRANSFORM() = default;
                    uint8_t transformType;
                };
                struct TRANSFORM0 : public TRANSFORM {
                    double scale = 1;
                };
                struct TRANSFORM1 : public TRANSFORM {
                    double scale;
                };
                struct TRANSFORM2 : public TRANSFORM {
                    qbn::vec2<double> scale;
                };
                struct TRANSFORM3 : public TRANSFORM {
                    qbn::mat<double, 2> scale;
                };

                TRANSFORM* transform{nullptr};
            };

            int16_t numberOfContours; // 'If the number of contours is positive or zero, it is a simple glyph; If the number of contours less than zero, the glyph is compound'
            qbn::vec2<FWord> min; // Minimum coordinate data
            qbn::vec2<FWord> max; // Maximum coordinate data
            COMPONENTGLYPH* component_glyphs{nullptr};
            uint32_t numComponentGlyphs;
        };

        uint32_t offset;
        GLYPH** glyphs{nullptr};
        uint32_t numGlyphs;
    };
    struct HEAD { // 'font header'
        uint32_t offset;
        Fixed version; // '0x00010000 if (version 1.0)'
        Fixed fontRevision; // 'set by font manufacturer'
        uint32_t checkSumAdjustment; // Check documentation for how to compute 'https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6head.html'
        uint32_t magicNumber; // 'set to 0x5F0F3CF5'
        uint16_t flags;
        // FLAG BITS
        // bit 0 = y value of 0 specifies baseline
        // bit 1 = x position of left most black bit is LSB
        // bit 2 = scaled point size and actual point size will differ
        // bit 3 = use integer scaling instead of fractional
        // bit 4 = (used by the Microsoft implement of the TrueType scaler)
        // bit 5 = (This bit should be set in fonts that are intended be laid out vertically, and in which the glyphs have been drawn such that an x-coordinate of 0 corresponds to the desired vertical baseline.)
        // bit 6 = This bit must be set to zero
        // bit 7 = This bit should be set if the font requires layout for correct linguistic rendering (e.g. Arabic fonts)
        // bit 8 = This bit should be set for an AAT font which has one or more metamorphois effects designated as happening by default
        // bit 9 = This bit should be set if the font contains any strong right-to-left glyphs
        // bit 10 = This bit should be set if the font contains Indic-style rearrangement effects
        // bit 11-13 Defined by adobe
        // bit 14 = This bit should be set ift he glyphs in the font are simply generic symbols for code point ranges, such as for a last resort font
        uint16_t unitsPerEm; // 'Range from 64 to 16384'
        longDateTime created; // 'international date'
        longDateTime modified; // 'international date'
        qbn::vec<FWord, 2> min; // 'For all glyph bounding boxes'
        qbn::vec<FWord, 2> max; // 'For all glyph bounding boxes'
        uint16_t macStyle; // Check documentation
        uint16_t lowestRecPPEM; // 'smallest readable size in pixels'
        int16_t fontDirectionHint; // Check documentation
        int16_t indexToLocFormat; // '0 for short offsets, 1 for long'
        int16_t glyphDataFormat; // '0 for current format'
    };
    struct HHEA { // 'horizontal header'
        uint32_t offset;
        Fixed version; // '0x00010000 (1.0)'
        FWord ascent; // 'Distance from baseline of highest ascender'
        FWord descent; // 'Distance from baseline of lowest descender'
        FWord lineGap; // 'typographic line gap'
        uFWord advanceWidthMax; // 'must be consistent with horizontal metric'
        FWord minLeftSideBearing; // 'must be consistent with horizontal metric'
        FWord minRightSideBearing; // 'must be consistent with horizontal metric'
        FWord xMaxExtent; // 'max(lsb + (xMax - xMin))'
        int16_t caretSlopeRise; // 'used to calculate the slope of the caret (rise/run) set to 1 for vertical caret'
        int16_t caretSlopeRun; // '0 for vertical'
        FWord caretOffset; // 'set value to 0 for non-slanted fonts'
        int64_t reserved; // 'set value to 0'
        int16_t metricDataFormat; // '0 for current format'
        uint16_t numOfLongHorMetrics; // 'number of advance widths in metrics table'
    };
    struct HMTX { // 'horizontal metrics'
        ~HMTX() {
            if (hMetrics != nullptr) {
                delete[] hMetrics;
            }
            if (leftSideBearing != nullptr) {
                delete[] leftSideBearing;
            }
        }
        struct LONGHORMETRIC {
            uint16_t advanceWidth;
            int16_t leftSideBearing;
        };

        uint32_t offset;
        LONGHORMETRIC* hMetrics{nullptr}; // Number of longHorMetrics comes from 'hhea' table (numOfLongHorMetrics)
        FWord* leftSideBearing{nullptr}; // 'The number of entries in this array is derived from the total number of glyphs minus numOfLongHorMetrics'
    };
    struct LOCA { // 'index to location'
        ~LOCA() {
            if (glyphOffsets != nullptr) {
                delete[] glyphOffsets;
            }
        }
        uint32_t offset;
        uint32_t* glyphOffsets{nullptr}; // num = num glyphs
    };
    struct MAXP { // 'maximum profile'
        uint32_t offset;
        Fixed version; // '0x00010000 (1.0)'
        uint16_t numGlyphs; // 'the number of glyphs in the font'
        uint16_t maxPoints; // 'points in non-compound glyph'
        uint16_t maxContours; // 'contours in non-compound glyph'
        uint16_t maxComponentPoints; // 'points in compound glyph'
        uint16_t maxComponentContours; // 'contours in compound glyph'
        uint16_t maxZones; // 'set to 2'
        uint16_t maxTwilightPoints; // 'points used in Twilight Zone (Z0)'
        uint16_t maxStorage; // 'number of Storage Area locations'
        uint16_t maxFunctionDefs; // 'number of FDEFs'
        uint16_t maxInstructionDefs; // 'number of IDEFs'
        uint16_t maxStackElements; // 'maximum stack depth'
        uint16_t maxSizeOfInstructions; // 'byte count for glyph instructions'
        uint16_t maxComponentElements; // 'number of glyphs referenced at top level'
        uint16_t maxComponentDepth; // 'levels of recursion, set to 0 if font only has simple glyphs'
    };
    struct NAME { // 'naming'
        ~NAME() {
            if (nameRecords != nullptr) {
                delete[] nameRecords;
            }
        }
        struct NAMERECORD {
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
        NAMERECORD* nameRecords{nullptr};
    };
    struct POST { // 'PostScript'
        uint32_t offset;
        Fixed format; // 'Format of this table' (check documentation for information about formats: https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6post.html)
        Fixed italicAngle; // 'Italic angle in degrees'
        FWord underlinePosition; // 'Underline position'
        FWord underlineThickness; // 'Underline thickness'
        uint32_t isFixedPitch; // 'set to 1 if font is monospaced and 0 otherwise'
        uint32_t minMemType42; // 'Minimum memory usage when a TrueType font is downloaded as a Type 42 font'
        uint32_t maxMemType42; // 'Maximum memory usage when a TrueType font is downloaded as a Type 42 font'
        uint32_t minMemType1; // 'Minimum memory usage when a TrueType font is downloaded as a Type 1 font'
        uint32_t maxMemType1; // 'Maximum memory usage when a TrueType font is downloaded as a Type 1 font'
    };
    struct KERN {
        ~KERN() {
            for (int i = 0; i < nTables; ++i) {
                delete subtables[i];
            }
            if (subtables != nullptr) {
                delete[] subtables;
            }
        }
        uint32_t offset{0};
        uint32_t version; // 'The version number of the kerning table (0x00010000 for the current version).'
        uint32_t nTables{0}; // 'The number of subtables included in the kerning table.'

        struct SUBTABLE {
            virtual ~SUBTABLE() = default;
            uint8_t format;
            uint32_t length; // 'The length of the subtable in bytes, including this header.'
            uint16_t tupleIndex; // 'The tuple index (used for variations fonts). This value specifies which tuple this subtable covers.'
            // ----------------------------------------------------------------------------------------
            //                                    COVERAGE TABLE
            // ----------------------------------------------------------------------------------------
            // Mask Value | Name            | Description
            // ----------------------------------------------------------------------------------------
            // 0x8000     | kernVertical    | Set if table has vertical kerning values.
            // 0x4000     | kernCrossStream | Set if table has cross-stream kerning values.
            // 0x2000     | kernVariation   | Set if table has variation kerning values.
            // 0x1F00     | kernUnusedBits  | Set to 0.
            // 0x00FF     | kernFormatMask  | Set the format of this subtable (0-3 currently defined).
            // ----------------------------------------------------------------------------------------
            bool kernVertical;
            bool kernCrossStream;
            bool kernVariation;
        };

        // Format 0 is an ordered list of kerning pairs
        // Format 1 is a state table for contextual kerning of up to eight glyphs
        // Format 2 is a simple two-dimensional array of kerning values and a class subtable
        // Format 3 is a compact form of Format 2

        struct SUBTABLE0 : public SUBTABLE {
            ~SUBTABLE0() {
                if (pairs != nullptr) {
                    delete[] pairs;
                }
            }

            uint16_t nPairs; // 'The number of kerning pairs in this subtable.'
            uint16_t searchRange; // 'The largest power of two less than or equal to the value of nPairs, multiplied by the size in bytes of an entry in the subtable.'
            uint16_t entrySelector; // 'This is calculated as long2 of the largest power of two less than or equal to the value of nPairs. This value indicates how many iterations of the search loop have to be made. For example, in a list of eight items, there would be three iterations of the loop.'
            uint16_t rangeShift; // 'The value of nPairs minus the largest power of two less than or equal to nPairs. This is multiplied by the size in bytes of an entry in the table.'

            struct PAIR {
                uint16_t left; // 'The glyph index for the lefthand glyph in the kerning pair.'
                uint16_t right; // 'The glyph index for the righthand glyph in the kerning pair.'
                int16_t value; // 'The kerning value in FUnits for the left and right pair in FUnits. If this value is greater than zero, the glyphs are moved apart. If this value is less than zero, the glyphs are moved together.'
            };

            PAIR* pairs{nullptr};
        };
        
        struct SUBTABLE1 : public SUBTABLE { // Currently unsupported
            StateHeader stHeader; // 'The contextual kerning state table header.'
            uint16_t valueTable; // 'Offset in bytes from the beginning of the subtable to the beginning of the kerning table.'
        };

        struct SUBTABLE2 : public SUBTABLE { // Currently unsupported

        };

        struct SUBTABLE3 : public SUBTABLE { // Currently unsupported

        };

        SUBTABLE** subtables{nullptr};
    };

    typedef struct TrueTypeFont {
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
    } TTF;
}

#endif