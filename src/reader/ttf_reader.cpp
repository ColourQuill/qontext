#include <reader/ttf_reader.hpp>

// std
#include <iostream>
#include <algorithm>
#include <vector>

namespace qtxt {
    using SIMPLEGLYPH = GLYF::SIMPLEGLYPH;
    using COMPOUNDGLYPH = GLYF::COMPOUNDGLYPH;
    using COMPONENTGLYPH = COMPOUNDGLYPH::COMPONENTGLYPH;
    using TRANSFORM0 = COMPONENTGLYPH::TRANSFORM0;
    using TRANSFORM1 = COMPONENTGLYPH::TRANSFORM1;
    using TRANSFORM2 = COMPONENTGLYPH::TRANSFORM2;
    using TRANSFORM3 = COMPONENTGLYPH::TRANSFORM3;
    using GLYPH = GLYF::GLYPH;

    using FORMAT0 = CMAP::FORMAT0;
    using FORMAT2 = CMAP::FORMAT2;
    using FORMAT4 = CMAP::FORMAT4;
    using FORMAT6 = CMAP::FORMAT6;
    using FORMAT8 = CMAP::FORMAT8;
    using FORMAT10 = CMAP::FORMAT10;
    using FORMAT12 = CMAP::FORMAT12;
    using FORMAT13 = CMAP::FORMAT13;
    using FORMAT14 = CMAP::FORMAT14;
    using FORMAT = CMAP::FORMAT;
    using SUBTABLE = CMAP::SUBTABLE;

    // *********************************************************************************************
    //                                      FUNCTION DEFINITIONS
    // *********************************************************************************************

    // Determines if a bit at a given index of a flag (8-bit) is on or off
    bool FlagBitIsOn(const uint8_t flag, int index) {
        return ((flag >> index) & 1) == 1;
    }
    bool FlagBitIsOn(const uint16_t flag, int index) {
        return ((flag >> index) & 1) == 1;
    }
    bool HasTag(std::vector<std::string>& tags, std::string tag) {
        auto it = std::find(tags.begin(), tags.end(), tag);

        if (it != tags.end()) {
            return true;
        }

        return false;
    }

    // *********************************************************************************************
    //                                       CMAP FUNCTIONS
    // *********************************************************************************************

    FORMAT0* Format0(Reader<TTF>* reader) { // Not tested
        FORMAT0* format0 = new FORMAT0();
        format0->length = reader->ReadUInt16();
        format0->language = reader->ReadUInt16();
        format0->glyphIndexArraySize = 256;
        format0->glyphIndexArray = new uint8_t[256];
        for (int i = 0; i < 256; ++i) {
            format0->glyphIndexArray[i] = reader->ReadUInt8();
        }
        return format0;
    }
    FORMAT2* Format2(Reader<TTF>* reader) { // Not tested
        FORMAT2* format2 = new FORMAT2();
        format2->length = reader->ReadUInt16();
        format2->language = reader->ReadUInt16();
        uint32_t numSubHeaders = 0;
        format2->subHeaderKeys = new uint16_t[256];
        for (int i = 0; i < 256; ++i) {
            uint16_t subHeaderKey = reader->ReadUInt16();
            uint16_t subHeaderIndex = subHeaderKey / 8;
            if (subHeaderIndex > numSubHeaders) {
                numSubHeaders = subHeaderIndex;
            }
            format2->subHeaderKeys[i] = subHeaderKey;
        }
        numSubHeaders++;
        format2->subHeaders = new FORMAT2::SUBHEADER[numSubHeaders];
        for (int i = 0; i < numSubHeaders; ++i) {
            FORMAT2::SUBHEADER subheader;
            subheader.firstCode = reader->ReadUInt16();
            subheader.entryCount = reader->ReadUInt16();
            subheader.idDelta = reader->ReadInt16();
            subheader.idRangeOffset = reader->ReadUInt16();
            format2->subHeaders[i] = subheader;
        }
        uint32_t glyphIndexArraySize = (format2->length - 518 - numSubHeaders * 8) / 2; // (total length - size of format, length, language, and subHeaderKeys in bytes - subHeaders in bytes) / 2
        format2->glyphIndexArraySize = glyphIndexArraySize;
        format2->glyphIndexArray = new uint16_t[glyphIndexArraySize];
        for (int i = 0; i < glyphIndexArraySize; ++i) {
            format2->glyphIndexArray[i] = reader->ReadUInt16();
        }
        return format2;
    }
    FORMAT4* Format4(Reader<TTF>* reader) { // Not tested
        FORMAT4* format4 = new FORMAT4();
        format4->length = reader->ReadUInt16();
        format4->language = reader->ReadUInt16();
        format4->segCountX2 = reader->ReadUInt16();
        format4->searchRange = reader->ReadUInt16();
        format4->entrySelector = reader->ReadUInt16();
        format4->rangeShift = reader->ReadUInt16();

        uint16_t segCount = format4->segCountX2 / 2;
        format4->segCount = segCount;

        format4->endCode = new uint16_t[segCount];
        format4->startCode = new uint16_t[segCount];
        format4->idDelta = new uint16_t[segCount];
        format4->idRangeOffset = new uint16_t[segCount];
        for (int i = 0; i < segCount; ++i) {
            format4->endCode[i] = reader->ReadUInt16();
        }

        format4->reservedPad = reader->ReadUInt16();

        for (int i = 0; i < segCount; ++i) {
            format4->startCode[i] = reader->ReadUInt16();
        }
        for (int i = 0; i < segCount; ++i) {
            format4->idDelta[i] = reader->ReadUInt16();
        }
        for (int i = 0; i < segCount; ++i) {
            format4->idRangeOffset[i] = reader->ReadUInt16();
        }

        uint32_t glyphIndexArraySize = (format4->length - 16 - 4 * format4->segCountX2) / 2;
        format4->glyphIndexArray = new uint16_t[glyphIndexArraySize];
        for (int i = 0; i < glyphIndexArraySize; ++i) {
            format4->glyphIndexArray[i] = reader->ReadUInt16();
        }

        return format4;
    }
    FORMAT6* Format6(Reader<TTF>* reader) { // Not tested
        FORMAT6* format6 = new FORMAT6();
        format6->length = reader->ReadUInt16();
        format6->language = reader->ReadUInt16();
        format6->firstCode = reader->ReadUInt16();
        format6->entryCount = reader->ReadUInt16();
        format6->glyphIndexArray = new uint16_t[format6->entryCount];
        for (int i = 0; i < format6->entryCount; ++i) {
            format6->glyphIndexArray[i] = reader->ReadUInt16();
        }
        return format6;
    }
    FORMAT8* Format8(Reader<TTF>* reader) { // Not tested
        FORMAT8* format8 = new FORMAT8();
        format8->reserved = reader->ReadUInt16();
        format8->length = reader->ReadUInt32();
        format8->langugage = reader->ReadUInt32();
        format8->is32Size = 8192;
        format8->is32 = new uint8_t[8192];
        for (int i = 0; i < 8192; ++i) {
            format8->is32[i] = reader->ReadUInt8();
        }
        format8->nGroups = reader->ReadUInt32();
        format8->groups = new FORMAT8::GROUP[format8->nGroups];
        for (int i = 0; i < format8->nGroups; ++i) {
            FORMAT8::GROUP group;
            group.startCharCode = reader->ReadUInt32();
            group.endCharCode = reader->ReadUInt32();
            group.startGlyphCode = reader->ReadUInt32();
            format8->groups[i] = group;
        }
        return format8;
    }
    FORMAT10* Format10(Reader<TTF>* reader) { // Not tested
        FORMAT10* format10 = new FORMAT10();
        format10->reserved = reader->ReadUInt16();
        format10->length = reader->ReadUInt32();
        format10->language = reader->ReadUInt32();
        format10->startCharCode = reader->ReadUInt32();
        format10->numChars = reader->ReadUInt32();
        format10->glyphs = new uint16_t[format10->numChars];
        for (int i = 0; i < format10->numChars; ++i) {
            format10->glyphs[i] = reader->ReadUInt16();
        }
        return format10;
    }
    FORMAT12* Format12(Reader<TTF>* reader) { // Not tested
        FORMAT12* format12 = new FORMAT12();
        format12->reserved = reader->ReadUInt16();
        format12->length = reader->ReadUInt32();
        format12->language = reader->ReadUInt32();
        format12->nGroups = reader->ReadUInt32();
        format12->groups = new FORMAT12::GROUP[format12->nGroups];
        for (int i = 0; i < format12->nGroups; ++i) {
            FORMAT12::GROUP group;
            group.startCharCode = reader->ReadUInt32();
            group.endCharCode = reader->ReadUInt32();
            group.startGlyphCode = reader->ReadUInt32();
            format12->groups[i] = group;
        }
        return format12;
    }
    FORMAT13* Format13(Reader<TTF>* reader) { // Not tested
        FORMAT13* format13 = new FORMAT13();
        format13->reserved = reader->ReadUInt16();
        format13->length = reader->ReadUInt32();
        format13->language = reader->ReadUInt32();
        format13->nGroups = reader->ReadUInt32();
        format13->groups = new FORMAT13::GROUP[format13->nGroups];
        for (int i = 0; i < format13->nGroups; ++i) {
            FORMAT13::GROUP group;
            group.startCharCode = reader->ReadUInt32();
            group.endCharCode = reader->ReadUInt32();
            group.startGlyphCode = reader->ReadUInt32();
            format13->groups[i] = group;
        }
        return format13;
    }
    FORMAT14* Format14(Reader<TTF>* reader) { // Not tested
        FORMAT14* format14 = new FORMAT14();
        format14->length = reader->ReadUInt32();
        format14->numVarSelectorRecords = reader->ReadUInt32();
        Endianness endianness = GetEndianness();
        format14->variationSelectorRecords = new FORMAT14::VARIATIONSELECTORRECORD[format14->numVarSelectorRecords];
        for (int i = 0; i < format14->numVarSelectorRecords; ++i) {
            FORMAT14::VARIATIONSELECTORRECORD variationSelectorRecord;
            unsigned char varSelectorA = reader->ReadUChar();
            unsigned char varSelectorB = reader->ReadUChar();
            unsigned char varSelectorC = reader->ReadUChar();
            unsigned char varSelectors[3] = {varSelectorA, varSelectorB, varSelectorC};
            if (endianness == Endianness::BIGENDIAN) {
                std::reverse(varSelectors, varSelectors + 3);
            }
            variationSelectorRecord.varSelector = static_cast<uint32_t>(CombineBytes(varSelectors, 3));
            variationSelectorRecord.defaultUVSOffset = reader->ReadUInt32();
            variationSelectorRecord.nonDefaultUVSOffset = reader->ReadUInt32();
            format14->variationSelectorRecords[i] = variationSelectorRecord;
        }

        for (int i = 0; i < format14->numVarSelectorRecords; ++i) {
            FORMAT14::VARIATIONSELECTORRECORD& record = format14->variationSelectorRecords[i];
            if (record.defaultUVSOffset != 0) {
                reader->SetCurrentByte(record.defaultUVSOffset);
                format14->defaultUVSTable.numUnicodeValueRanges = reader->ReadUInt32();
                format14->defaultUVSTable.unicodeValueRanges = new FORMAT14::DEFAULTUVSTABLE::UNICODEVALUERANGE[format14->defaultUVSTable.numUnicodeValueRanges];
                for (int i = 0; i < format14->defaultUVSTable.numUnicodeValueRanges; ++i) {
                    FORMAT14::DEFAULTUVSTABLE::UNICODEVALUERANGE unicodeValueRange;
                    unsigned char startUnicodeValueA = reader->ReadUChar();
                    unsigned char startUnicodeValueB = reader->ReadUChar();
                    unsigned char startUnicodeValueC = reader->ReadUChar();
                    unsigned char startUnicodeValues[3] = {startUnicodeValueA, startUnicodeValueB, startUnicodeValueC};
                    if (endianness == Endianness::BIGENDIAN) {
                        std::reverse(startUnicodeValues, startUnicodeValues + 3);
                    }
                    unicodeValueRange.startUnicodeValue = static_cast<uint32_t>(CombineBytes(startUnicodeValues, 3));
                    unicodeValueRange.additionalCount = reader->ReadUInt8();
                    format14->defaultUVSTable.unicodeValueRanges[i] = unicodeValueRange;
                }
            }
            if (record.nonDefaultUVSOffset != 0) {
                reader->SetCurrentByte(record.nonDefaultUVSOffset);
                format14->nonDefaultUVSTable.numUVSMappings = reader->ReadUInt32();
                format14->nonDefaultUVSTable.uvsMappings = new FORMAT14::NONDEFAULTUVSTABLE::UVSMAPPING[format14->nonDefaultUVSTable.numUVSMappings];
                for (int i = 0; i < format14->nonDefaultUVSTable.numUVSMappings; ++i) {
                    FORMAT14::NONDEFAULTUVSTABLE::UVSMAPPING uvsMapping;
                    unsigned char unicodeValueA = reader->ReadUChar();
                    unsigned char unicodeValueB = reader->ReadUChar();
                    unsigned char unicodeValueC = reader->ReadUChar();
                    unsigned char unicodeValues[3] = {unicodeValueA, unicodeValueB, unicodeValueC};
                    if (endianness == Endianness::BIGENDIAN) {
                        std::reverse(unicodeValues, unicodeValues + 3);
                    }
                    uvsMapping.unicodeValue = static_cast<uint32_t>(CombineBytes(unicodeValues, 3));
                    uvsMapping.glyphID = reader->ReadUInt16();
                    format14->nonDefaultUVSTable.uvsMappings[i] = uvsMapping;
                }
            }
        }
        return format14;
    }
    // *********************************************************************************************
    //                                       GLYF FUNCTIONS
    // *********************************************************************************************

    SIMPLEGLYPH::POINT* ReadPoints(Reader<TTF>* reader, SIMPLEGLYPH* simple_glyph) {
       SIMPLEGLYPH::POINT* points = new SIMPLEGLYPH::POINT[simple_glyph->numFlags];
        for (int i = 0; i < simple_glyph->numFlags; ++i) {
            if (i != 0) {
                points[i].coordinates.x = points[i - 1].coordinates.x;
            } else {
                points[i].coordinates.x = 0;
            }

            uint8_t flag = simple_glyph->flags[i];
            bool onCurve = FlagBitIsOn(flag, 0);
            points[i].onCurve = onCurve;

            if (FlagBitIsOn(flag, 1)) {
                uint8_t offset = reader->ReadUInt8();
                int16_t sign = FlagBitIsOn(flag, 4) ? 1 : -1;
                points[i].coordinates.x += (FWord)offset * (FWord)sign;
            } else if (!FlagBitIsOn(flag, 4)) {
                points[i].coordinates.x += reader->ReadInt16();
            }
        }
        for (int i = 0; i < simple_glyph->numFlags; ++i) {
            if (i != 0) {
                points[i].coordinates.y = points[i - 1].coordinates.y;
            } else {
                points[i].coordinates.y = 0;
            }

            uint8_t flag = simple_glyph->flags[i];

            if (FlagBitIsOn(flag, 2)) {
                uint8_t offset = reader->ReadUInt8();
                int16_t sign = FlagBitIsOn(flag, 5) ? 1 : -1;
                points[i].coordinates.y += (FWord)offset * (FWord)sign;
            } else if (!FlagBitIsOn(flag, 5)) {
                points[i].coordinates.y += reader->ReadInt16();
            }
        }
        return points;
    }
    void ReadSimpleGlyph(SIMPLEGLYPH* simple_glyph, Reader<TTF>* reader, int16_t numberOfContours) {
        simple_glyph->numberOfContours = numberOfContours;
        if (numberOfContours == 0) { return; }
        FWord xMin = reader->ReadFWord();
        FWord yMin = reader->ReadFWord();
        FWord xMax = reader->ReadFWord();
        FWord yMax = reader->ReadFWord();
        simple_glyph->min = qbn::vec2<FWord>{xMin, yMin};
        simple_glyph->max = qbn::vec2<FWord>{xMax, yMax};
        simple_glyph->endPtsOfContours = new uint16_t[numberOfContours];
        for (int i = 0; i < numberOfContours; ++i) {
            simple_glyph->endPtsOfContours[i] = reader->ReadUInt16();
        }
        simple_glyph->instructionLength = reader->ReadUInt16();
        simple_glyph->instructions = new uint8_t[simple_glyph->instructionLength];
        for (int i = 0; i < simple_glyph->instructionLength; ++i) {
            simple_glyph->instructions[i] = reader->ReadUInt8();
        }
        uint16_t numPoints = simple_glyph->endPtsOfContours[numberOfContours - 1] + 1;
        simple_glyph->numFlags = numPoints;
        simple_glyph->flags = new uint8_t[numPoints];
        for (int i = 0; i < numPoints; ++i) {
            uint8_t flag = reader->ReadUInt8();
            simple_glyph->flags[i] = flag;

            if (FlagBitIsOn(flag, 3)) {
                uint8_t numRepeats = reader->ReadUInt8();
                uint8_t k = i;
                for (int j = 0; j < numRepeats; ++j) {
                    simple_glyph->flags[k + j + 1] = flag;
                    ++i;
                }
            }
        }
        simple_glyph->points = ReadPoints(reader, simple_glyph);
    }
    void ReadComponentGlyph(COMPONENTGLYPH& component_glyph, Reader<TTF>* reader) {
        component_glyph.flags = reader->ReadUInt16();
        component_glyph.glyphIndex = reader->ReadUInt16();
        int16_t xOffset = FlagBitIsOn(component_glyph.flags, 0) ? reader->ReadInt16() : reader->ReadInt8();
        int16_t yOffset = FlagBitIsOn(component_glyph.flags, 0) ? reader->ReadInt16() : reader->ReadInt8();
        component_glyph.offset = qbn::vec2<int16_t>{xOffset, yOffset};
        if (FlagBitIsOn(component_glyph.flags, 3)) {
            COMPONENTGLYPH::TRANSFORM1* transform = new TRANSFORM1();
            transform->transformType = 1;
            transform->scale = reader->ReadFixedPoint(2, 14);
            component_glyph.transform = transform;
        } else if (FlagBitIsOn(component_glyph.flags, 6)) {
            COMPONENTGLYPH::TRANSFORM2* transform = new TRANSFORM2();
            transform->transformType = 2;
            double scaleX = reader->ReadFixedPoint(2, 14);
            double scaleY = reader->ReadFixedPoint(2, 14);
            transform->scale = {scaleX, scaleY};
            component_glyph.transform = transform;
        } else if (FlagBitIsOn(component_glyph.flags, 7)) {
            COMPONENTGLYPH::TRANSFORM3* transform = new TRANSFORM3();
            transform->transformType = 2;
            double scaleXX = reader->ReadFixedPoint(2, 14);
            double scaleXY = reader->ReadFixedPoint(2, 14);
            double scaleYX = reader->ReadFixedPoint(2, 14);
            double scaleYY = reader->ReadFixedPoint(2, 14);
            transform->scale = {{scaleXX, scaleYX}, {scaleXY, scaleYY}};
            component_glyph.transform = transform;
        } else {
            COMPONENTGLYPH::TRANSFORM0* transform = new TRANSFORM0();
            transform->transformType = 0;
            transform->scale = 1;
            component_glyph.transform = transform;
        }
    }
    void ReadCompoundGlyph(COMPOUNDGLYPH* compound_glyph, Reader<TTF>* reader, int16_t numberOfContours) {
        compound_glyph->numberOfContours = numberOfContours;
        FWord xMin = reader->ReadFWord();
        FWord yMin = reader->ReadFWord();
        FWord xMax = reader->ReadFWord();
        FWord yMax = reader->ReadFWord();
        compound_glyph->min = qbn::vec2<FWord>{xMin, yMin};
        compound_glyph->max = qbn::vec2<FWord>{xMax, yMax};

        int numComponentGlyphs = 0;
        unsigned long long position = reader->GetPosition();
        while (true) {
            COMPONENTGLYPH component_glyph{};
            ReadComponentGlyph(component_glyph, reader);
            ++numComponentGlyphs;
            if (!FlagBitIsOn(component_glyph.flags, 5)) {
                break;
            }
        }
        compound_glyph->numComponentGlyphs = numComponentGlyphs;
        compound_glyph->component_glyphs = new COMPONENTGLYPH[numComponentGlyphs];

        reader->SetCurrentByte(position);
        for (int i = 0; i < numComponentGlyphs; ++i) {
            ReadComponentGlyph(compound_glyph->component_glyphs[i], reader);
        }
    }
    GLYPH* ReadGlyph(Reader<TTF>* reader, uint32_t location, int index) {
        reader->SetCurrentByte(location);
        int16_t numberOfContours = reader->ReadInt16();
        if (numberOfContours < 0) {
            COMPOUNDGLYPH* compound_glyph = new COMPOUNDGLYPH();
            ReadCompoundGlyph(compound_glyph, reader, numberOfContours);
            compound_glyph->glyphType = 1;
            return compound_glyph;
        } else {
            SIMPLEGLYPH* simple_glyph = new SIMPLEGLYPH();
            ReadSimpleGlyph(simple_glyph, reader, numberOfContours);
            simple_glyph->glyphType = 0;
            return simple_glyph;
        }
    }

    // *********************************************************************************************
    //                                       KERN FUNCTIONS
    // *********************************************************************************************

    KERN::SUBTABLE0* Subtable0(Reader<TTF>* reader) {
        KERN::SUBTABLE0* subtable0 = new KERN::SUBTABLE0();

        subtable0->nPairs = reader->ReadUInt16();
        subtable0->searchRange = reader->ReadUInt16();
        subtable0->entrySelector = reader->ReadUInt16();
        subtable0->rangeShift = reader->ReadUInt16();

        subtable0->pairs = new KERN::SUBTABLE0::PAIR[subtable0->nPairs];

        for (int i = 0; i < subtable0->nPairs; ++i) {
            KERN::SUBTABLE0::PAIR pair;
            pair.left = reader->ReadUInt16();
            pair.right = reader->ReadUInt16();
            pair.value = reader->ReadInt16();

            subtable0->pairs[i] = pair;
        }

        return subtable0;
    }

    // *********************************************************************************************
    //                       TRUETYPEFONT PRIVATE READER STRUCT DEFINITIONS
    // *********************************************************************************************

    // Sets the offsets of all the tables
    std::vector<std::string> Reader<TTF>::TableOffsets(TTF& ttf) {
        std::vector<std::string> tags;
        ResetCurrentByte();
        SkipBytes(4);
        uint16_t numTables = ReadUInt16();
        SkipBytes(6);
        for (int i = 0; i < numTables; ++i) {
            std::string tag = ReadString(4);
            tags.push_back(tag);
            SkipBytes(4);
            uint32_t offset = ReadUInt32();
            SkipBytes(4);
            if (tag == "cmap") { ttf.cmap.offset = offset; continue; }
            if (tag == "glyf") { ttf.glyf.offset = offset; continue; }
            if (tag == "head") { ttf.head.offset = offset; continue; }
            if (tag == "hhea") { ttf.hhea.offset = offset; continue; }
            if (tag == "hmtx") { ttf.hmtx.offset = offset; continue; }
            if (tag == "loca") { ttf.loca.offset = offset; continue; }
            if (tag == "maxp") { ttf.maxp.offset = offset; continue; }
            if (tag == "name") { ttf.name.offset = offset; continue; }
            if (tag == "post") { ttf.post.offset = offset; continue; }
            if (tag == "kern") { ttf.kern.offset = offset; continue; }
        }
        ResetCurrentByte();
        return tags;
    }
    
    // Initializes the tables
    void Reader<TTF>::LoadTables(TTF& ttf) {
        std::vector<std::string> tags = TableOffsets(ttf);

        // Required
        LoadCMAP(ttf);
        LoadHEAD(ttf);
        LoadMAXP(ttf);
        LoadLOCA(ttf);
        LoadGLYF(ttf);
        LoadHHEA(ttf);
        LoadHMTX(ttf);
        LoadNAME(ttf);
        LoadPOST(ttf);

        // Optional
        if (HasTag(tags, "kern")) {
            LoadKERN(ttf);
        }

        ClearBuffer();
    }
    
    // Initializes the cmap table
    void Reader<TTF>::LoadCMAP(TTF& ttf) {
        SetCurrentByte(ttf.cmap.offset);
        ttf.cmap.version = ReadUInt16();
        ttf.cmap.numberSubtables = ReadUInt16();

        std::vector<SUBTABLE> validSubtables;
        for (int i = 0; i < ttf.cmap.numberSubtables; ++i) {
            SUBTABLE subtable;
            subtable.platformID = ReadUInt16();
            subtable.platformSpecificID = ReadUInt16();
            subtable.offset = ReadUInt32();

            bool validPlatformID = false;
            switch (subtable.platformID) {
                case 0:
                    switch (subtable.platformSpecificID) { // Unicode
                        case 0: break;
                        case 1: break;
                        case 2: break;
                        case 3: validPlatformID = true; break;
                        case 4: validPlatformID = true; break;
                        case 5: break;
                        case 6: break;
                        default: break;
                    }
                case 1: break; // Macintosh
                case 2: break; // Reserved
                case 3: break; // Microsoft
                default: break;
            }
            if (validPlatformID) {
                validSubtables.push_back(subtable);
            }
        }
        if (validSubtables.size() == 0) {
            throw std::runtime_error("No valid subtables were found.");
        }

        ttf.cmap.numberSubtables = validSubtables.size();
        ttf.cmap.subtables = new CMAP::SUBTABLE[validSubtables.size()];

        int i = 0;
        for (SUBTABLE& validSubtable : validSubtables) {
            SUBTABLE subtable;
            subtable.format = validSubtable.format;
            subtable.formatID = validSubtable.formatID;
            subtable.offset = validSubtable.offset;
            subtable.platformID = validSubtable.platformID;
            subtable.platformSpecificID = validSubtable.platformSpecificID;
            
            SetCurrentByte(ttf.cmap.offset + subtable.offset);
            subtable.formatID = ReadUInt16();
            switch (subtable.formatID) {
                case 0: subtable.format = Format0(this); break;
                case 2: subtable.format = Format2(this); break;
                case 4: subtable.format = Format4(this); break;
                case 6: subtable.format = Format6(this); break;
                case 8: subtable.format = Format8(this); break;
                case 10: subtable.format = Format10(this); break;
                case 12: subtable.format = Format12(this); break;
                case 13: subtable.format = Format13(this); break;
                case 14: subtable.format = Format14(this); break;
                default: std::runtime_error("Invalid format for subtable.");
            }
            ttf.cmap.subtables[i++] = subtable;
        }
    }
    // Initializes the head table
    void Reader<TTF>::LoadHEAD(TTF& ttf) {
        SetCurrentByte(ttf.head.offset);
        ttf.head.version = ReadFixed();
        ttf.head.fontRevision = ReadFixed();
        ttf.head.checkSumAdjustment = ReadUInt32();
        ttf.head.magicNumber = ReadUInt32();
        ttf.head.flags = ReadUInt16();
        ttf.head.unitsPerEm = ReadUInt16();
        ttf.head.created = ReadLongDateTime();
        ttf.head.modified = ReadLongDateTime();
        FWord xMin = ReadFWord();
        FWord yMin = ReadFWord();
        FWord xMax = ReadFWord();
        FWord yMax = ReadFWord();
        ttf.head.min = qbn::vec2<FWord>{xMin, yMin};
        ttf.head.max = qbn::vec2<FWord>{xMax, yMax};
        ttf.head.macStyle = ReadUInt16();
        ttf.head.lowestRecPPEM = ReadUInt16();
        ttf.head.fontDirectionHint = ReadInt16();
        ttf.head.indexToLocFormat = ReadInt16();
        ttf.head.glyphDataFormat = ReadInt16();
    }
    // Initializes the maxp table
    void Reader<TTF>::LoadMAXP(TTF& ttf) {
        SetCurrentByte(ttf.maxp.offset);
        ttf.maxp.version = ReadFixed();
        ttf.maxp.numGlyphs = ReadUInt16();
        ttf.maxp.maxPoints = ReadUInt16();
        ttf.maxp.maxContours = ReadUInt16();
        ttf.maxp.maxComponentPoints = ReadUInt16();
        ttf.maxp.maxComponentContours = ReadUInt16();
        ttf.maxp.maxZones = ReadUInt16();
        ttf.maxp.maxTwilightPoints = ReadUInt16();
        ttf.maxp.maxStorage = ReadUInt16();
        ttf.maxp.maxFunctionDefs = ReadUInt16();
        ttf.maxp.maxInstructionDefs = ReadUInt16();
        ttf.maxp.maxStackElements = ReadUInt16();
        ttf.maxp.maxSizeOfInstructions = ReadUInt16();
        ttf.maxp.maxComponentElements = ReadUInt16();
        ttf.maxp.maxComponentDepth = ReadUInt16();
    }
    // Initializes the loca table
    void Reader<TTF>::LoadLOCA(TTF& ttf) {
        SetCurrentByte(ttf.loca.offset);
        ttf.loca.glyphOffsets = new uint32_t[ttf.maxp.numGlyphs];
        for (int i = 0; i < ttf.maxp.numGlyphs; ++i) {
            uint32_t glyphOffset = (ttf.head.indexToLocFormat == 0) ? ReadUInt16() * 2u : ReadUInt32();
            ttf.loca.glyphOffsets[i] = glyphOffset; // Offset from ttf.glyf.offset
        }
    }
    // Initializes the glyf table
    void Reader<TTF>::LoadGLYF(TTF& ttf) {
        ttf.glyf.glyphs = new GLYPH*[ttf.maxp.numGlyphs];
        ttf.glyf.numGlyphs = ttf.maxp.numGlyphs;
        for (int i = 0; i < ttf.maxp.numGlyphs; ++i) {
            GLYPH* glyph = ReadGlyph(this, ttf.loca.glyphOffsets[i] + ttf.glyf.offset, i);
            ttf.glyf.glyphs[i] = glyph;
        }
    }
    // Initializes the hhea table
    void Reader<TTF>::LoadHHEA(TTF& ttf) {
        SetCurrentByte(ttf.hhea.offset);
        ttf.hhea.version = ReadFixed();
        ttf.hhea.ascent = ReadFWord();
        ttf.hhea.descent = ReadFWord();
        ttf.hhea.lineGap = ReadFWord();
        ttf.hhea.advanceWidthMax = ReadUFWord();
        ttf.hhea.minLeftSideBearing = ReadFWord();
        ttf.hhea.minRightSideBearing = ReadFWord();
        ttf.hhea.xMaxExtent = ReadFWord();
        ttf.hhea.caretSlopeRise = ReadInt16();
        ttf.hhea.caretSlopeRun = ReadInt16();
        ttf.hhea.caretOffset = ReadFWord();
        ttf.hhea.reserved = ReadInt64();
        ttf.hhea.metricDataFormat = ReadInt16();
        ttf.hhea.numOfLongHorMetrics = ReadUInt16();
    }
    // Initializes the hmtx table
    void Reader<TTF>::LoadHMTX(TTF& ttf) {
        SetCurrentByte(ttf.hmtx.offset);
        ttf.hmtx.hMetrics = new HMTX::LONGHORMETRIC[ttf.hhea.numOfLongHorMetrics];
        for (int i = 0; i < ttf.hhea.numOfLongHorMetrics; ++i) {
            HMTX::LONGHORMETRIC longHorMetric;
            longHorMetric.advanceWidth = ReadUInt16();
            longHorMetric.leftSideBearing = ReadInt16();
            ttf.hmtx.hMetrics[i] = longHorMetric;
        }
        uint16_t numBearings = ttf.maxp.numGlyphs - ttf.hhea.numOfLongHorMetrics;
        if (numBearings > 0) {
            ttf.hmtx.leftSideBearing = new FWord[numBearings];
            for (int i = 0; i < numBearings; ++i) {
                ttf.hmtx.leftSideBearing[i] = ReadFWord();
            }
        }
    }
    // Initializes the name table
    void Reader<TTF>::LoadNAME(TTF& ttf) {
        SetCurrentByte(ttf.name.offset);
        ttf.name.format = ReadUInt16();
        ttf.name.count = ReadUInt16();
        ttf.name.stringOffset = ReadUInt16();

        ttf.name.nameRecords = new NAME::NAMERECORD[ttf.name.count];
        for (int i = 0; i < ttf.name.count; ++i) {
            NAME::NAMERECORD nameRecord;
            nameRecord.platformID = ReadUInt16();
            nameRecord.platformSpecificID = ReadUInt16();
            nameRecord.languageID = ReadUInt16();
            nameRecord.nameID = ReadUInt16();
            nameRecord.length = ReadUInt16();
            nameRecord.offset = ReadUInt16();
            ttf.name.nameRecords[i] = nameRecord;
        }
        for (int i = 0; i < ttf.name.count; ++i) {
            NAME::NAMERECORD& nameRecord = ttf.name.nameRecords[i];
            SetCurrentByte(ttf.name.offset + ttf.name.stringOffset + nameRecord.offset);
            nameRecord.name = ReadString(nameRecord.length);
        }
    }
    // Initializes the post table
    void Reader<TTF>::LoadPOST(TTF& ttf) {
        SetCurrentByte(ttf.post.offset);
        ttf.post.format = ReadFixed();
        ttf.post.italicAngle = ReadFixed();
        ttf.post.underlinePosition = ReadFWord();
        ttf.post.underlineThickness = ReadFWord();
        ttf.post.isFixedPitch = ReadUInt32();
        ttf.post.minMemType42 = ReadUInt32();
        ttf.post.maxMemType42 = ReadUInt32();
        ttf.post.minMemType1 = ReadUInt32();
        ttf.post.maxMemType1 = ReadUInt32();
    }

    // Initializes the kern table
    void Reader<TTF>::LoadKERN(TTF& ttf) {
        SetCurrentByte(ttf.kern.offset);
        ttf.kern.version = ReadUInt16();
        if (ttf.kern.version == 0) {
            ttf.kern.nTables = ReadUInt16();
        } else {
            SetCurrentByte(ttf.kern.offset);
            ttf.kern.version = ReadFixed();
            ttf.kern.nTables = ReadUInt32();
        }

        ttf.kern.subtables = new KERN::SUBTABLE*[ttf.kern.nTables];

        for (int i = 0; i < ttf.kern.nTables; ++i) {
            uint32_t length = ReadUInt32();
            uint16_t coverage = ReadUInt16();
            uint16_t tupleIndex = ReadUInt16();

            bool kernVertical = FlagBitIsOn(coverage, 0);
            bool kernCrossStream = FlagBitIsOn(coverage, 1);
            bool kernVariation = FlagBitIsOn(coverage, 2);

            uint8_t format = (coverage << 8);

            if (format == 0) {
                KERN::SUBTABLE0* subtable0 = Subtable0(this);
                subtable0->length = length;
                subtable0->tupleIndex = tupleIndex;
                subtable0->format = format;
                subtable0->kernVertical = kernVertical;
                subtable0->kernCrossStream = kernCrossStream;
                subtable0->kernVariation = kernVariation;
                ttf.kern.subtables[i] = subtable0;
            }
        }
    }

    // *********************************************************************************************
    //                       TRUETYPEFONT PUBLIC READER STRUCT DEFINITIONS
    // *********************************************************************************************

    TTF Reader<TTF>::CreateTTFFromFile(const std::string& path) {
        return CreateTTFFromFile(path.c_str());
    }
    TTF Reader<TTF>::CreateTTFFromFile(const char* path) {
        ReadFile(path);
        TTF ttf{};
        LoadTables(ttf);
        return ttf;
    }
}