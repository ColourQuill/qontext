#include <truetypefont/truetypefont.hpp>

// std
#include <fstream>
#include <stdexcept>
#include <cstring>

// qontext
#include <utils/reader.hpp>

namespace qtxt {
    using Transform0 = ComponentGlyph::Transform0;
    using Transform1 = ComponentGlyph::Transform1;
    using Transform2 = ComponentGlyph::Transform2;
    using Transform3 = ComponentGlyph::Transform3;

    TrueTypeFont::TrueTypeFont(const std::string& path) {
        Reader<BIGENDIAN> reader{};

        reader.ReadFile(path);

        Init(reader);
    }

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

    Format0* GetFormat0(Reader<BIGENDIAN>& reader) { // Not tested
        Format0* format0 = new Format0();
        format0->length = reader.ReadUInt16();
        format0->language = reader.ReadUInt16();
        format0->glyphIndexArraySize = 256;
        format0->glyphIndexArray = new uint8_t[256];
        for (int i = 0; i < 256; ++i) {
            format0->glyphIndexArray[i] = reader.ReadUInt8();
        }
        return format0;
    }
    Format2* GetFormat2(Reader<BIGENDIAN>& reader) { // Not tested
        Format2* format2 = new Format2();
        format2->length = reader.ReadUInt16();
        format2->language = reader.ReadUInt16();
        uint32_t numSubHeaders = 0;
        format2->subHeaderKeys = new uint16_t[256];
        for (int i = 0; i < 256; ++i) {
            uint16_t subHeaderKey = reader.ReadUInt16();
            uint16_t subHeaderIndex = subHeaderKey / 8;
            if (subHeaderIndex > numSubHeaders) {
                numSubHeaders = subHeaderIndex;
            }
            format2->subHeaderKeys[i] = subHeaderKey;
        }
        numSubHeaders++;
        format2->subHeaders = new Format2::Subheader[numSubHeaders];
        for (int i = 0; i < numSubHeaders; ++i) {
            Format2::Subheader subheader;
            subheader.firstCode = reader.ReadUInt16();
            subheader.entryCount = reader.ReadUInt16();
            subheader.idDelta = reader.ReadInt16();
            subheader.idRangeOffset = reader.ReadUInt16();
            format2->subHeaders[i] = subheader;
        }
        uint32_t glyphIndexArraySize = (format2->length - 518 - numSubHeaders * 8) / 2; // (total length - size of format, length, language, and subHeaderKeys in bytes - subHeaders in bytes) / 2
        format2->glyphIndexArraySize = glyphIndexArraySize;
        format2->glyphIndexArray = new uint16_t[glyphIndexArraySize];
        for (int i = 0; i < glyphIndexArraySize; ++i) {
            format2->glyphIndexArray[i] = reader.ReadUInt16();
        }
        return format2;
    }
    Format4* GetFormat4(Reader<BIGENDIAN>& reader) { // Not tested
        Format4* format4 = new Format4();
        format4->length = reader.ReadUInt16();
        format4->language = reader.ReadUInt16();
        format4->segCountX2 = reader.ReadUInt16();
        format4->searchRange = reader.ReadUInt16();
        format4->entrySelector = reader.ReadUInt16();
        format4->rangeShift = reader.ReadUInt16();

        uint16_t segCount = format4->segCountX2 / 2;
        format4->segCount = segCount;

        format4->endCode = new uint16_t[segCount];
        format4->startCode = new uint16_t[segCount];
        format4->idDelta = new uint16_t[segCount];
        format4->idRangeOffset = new uint16_t[segCount];
        for (int i = 0; i < segCount; ++i) {
            format4->endCode[i] = reader.ReadUInt16();
        }

        format4->reservedPad = reader.ReadUInt16();

        for (int i = 0; i < segCount; ++i) {
            format4->startCode[i] = reader.ReadUInt16();
        }
        for (int i = 0; i < segCount; ++i) {
            format4->idDelta[i] = reader.ReadUInt16();
        }
        for (int i = 0; i < segCount; ++i) {
            format4->idRangeOffset[i] = reader.ReadUInt16();
        }

        uint32_t glyphIndexArraySize = (format4->length - 16 - 4 * format4->segCountX2) / 2;
        format4->glyphIndexArray = new uint16_t[glyphIndexArraySize];
        for (int i = 0; i < glyphIndexArraySize; ++i) {
            format4->glyphIndexArray[i] = reader.ReadUInt16();
        }

        return format4;
    }
    Format6* GetFormat6(Reader<BIGENDIAN>& reader) { // Not tested
        Format6* format6 = new Format6();
        format6->length = reader.ReadUInt16();
        format6->language = reader.ReadUInt16();
        format6->firstCode = reader.ReadUInt16();
        format6->entryCount = reader.ReadUInt16();
        format6->glyphIndexArray = new uint16_t[format6->entryCount];
        for (int i = 0; i < format6->entryCount; ++i) {
            format6->glyphIndexArray[i] = reader.ReadUInt16();
        }
        return format6;
    }
    Format8* GetFormat8(Reader<BIGENDIAN>& reader) { // Not tested
        Format8* format8 = new Format8();
        format8->reserved = reader.ReadUInt16();
        format8->length = reader.ReadUInt32();
        format8->langugage = reader.ReadUInt32();
        format8->is32Size = 8192;
        format8->is32 = new uint8_t[8192];
        for (int i = 0; i < 8192; ++i) {
            format8->is32[i] = reader.ReadUInt8();
        }
        format8->nGroups = reader.ReadUInt32();
        format8->groups = new Format8::Group[format8->nGroups];
        for (int i = 0; i < format8->nGroups; ++i) {
            Format8::Group group;
            group.startCharCode = reader.ReadUInt32();
            group.endCharCode = reader.ReadUInt32();
            group.startGlyphCode = reader.ReadUInt32();
            format8->groups[i] = group;
        }
        return format8;
    }
    Format10* GetFormat10(Reader<BIGENDIAN>& reader) { // Not tested
        Format10* format10 = new Format10();
        format10->reserved = reader.ReadUInt16();
        format10->length = reader.ReadUInt32();
        format10->language = reader.ReadUInt32();
        format10->startCharCode = reader.ReadUInt32();
        format10->numChars = reader.ReadUInt32();
        format10->glyphs = new uint16_t[format10->numChars];
        for (int i = 0; i < format10->numChars; ++i) {
            format10->glyphs[i] = reader.ReadUInt16();
        }
        return format10;
    }
    Format12* GetFormat12(Reader<BIGENDIAN>& reader) { // Not tested
        Format12* format12 = new Format12();
        format12->reserved = reader.ReadUInt16();
        format12->length = reader.ReadUInt32();
        format12->language = reader.ReadUInt32();
        format12->nGroups = reader.ReadUInt32();
        format12->groups = new Format12::Group[format12->nGroups];
        for (int i = 0; i < format12->nGroups; ++i) {
            Format12::Group group;
            group.startCharCode = reader.ReadUInt32();
            group.endCharCode = reader.ReadUInt32();
            group.startGlyphCode = reader.ReadUInt32();
            format12->groups[i] = group;
        }
        return format12;
    }
    Format13* GetFormat13(Reader<BIGENDIAN>& reader) { // Not tested
        Format13* format13 = new Format13();
        format13->reserved = reader.ReadUInt16();
        format13->length = reader.ReadUInt32();
        format13->language = reader.ReadUInt32();
        format13->nGroups = reader.ReadUInt32();
        format13->groups = new Format13::Group[format13->nGroups];
        for (int i = 0; i < format13->nGroups; ++i) {
            Format13::Group group;
            group.startCharCode = reader.ReadUInt32();
            group.endCharCode = reader.ReadUInt32();
            group.startGlyphCode = reader.ReadUInt32();
            format13->groups[i] = group;
        }
        return format13;
    }
    Format14* GetFormat14(Reader<BIGENDIAN>& reader) { // Not tested
        Format14* format14 = new Format14();
        format14->length = reader.ReadUInt32();
        format14->numVarSelectorRecords = reader.ReadUInt32();
        Endianness endianness = GetEndianness();
        format14->variationSelectorRecords = new Format14::VariationSelectorRecord[format14->numVarSelectorRecords];
        for (int i = 0; i < format14->numVarSelectorRecords; ++i) {
            Format14::VariationSelectorRecord variationSelectorRecord;
            unsigned char varSelectorA = reader.ReadUChar();
            unsigned char varSelectorB = reader.ReadUChar();
            unsigned char varSelectorC = reader.ReadUChar();
            unsigned char varSelectors[3] = {varSelectorA, varSelectorB, varSelectorC};
            if (endianness == Endianness::BIGENDIAN) {
                std::reverse(varSelectors, varSelectors + 3);
            }
            variationSelectorRecord.varSelector = static_cast<uint32_t>(CombineBytes(varSelectors, 3));
            variationSelectorRecord.defaultUVSOffset = reader.ReadUInt32();
            variationSelectorRecord.nonDefaultUVSOffset = reader.ReadUInt32();
            format14->variationSelectorRecords[i] = variationSelectorRecord;
        }

        for (int i = 0; i < format14->numVarSelectorRecords; ++i) {
            Format14::VariationSelectorRecord& record = format14->variationSelectorRecords[i];
            if (record.defaultUVSOffset != 0) {
                reader.SetCurrentByte(record.defaultUVSOffset);
                format14->defaultUVSTable.numUnicodeValueRanges = reader.ReadUInt32();
                format14->defaultUVSTable.unicodeValueRanges = new Format14::DefaultUVsTable::UnicodeValueRange[format14->defaultUVSTable.numUnicodeValueRanges];
                for (int i = 0; i < format14->defaultUVSTable.numUnicodeValueRanges; ++i) {
                    Format14::DefaultUVsTable::UnicodeValueRange unicodeValueRange;
                    unsigned char startUnicodeValueA = reader.ReadUChar();
                    unsigned char startUnicodeValueB = reader.ReadUChar();
                    unsigned char startUnicodeValueC = reader.ReadUChar();
                    unsigned char startUnicodeValues[3] = {startUnicodeValueA, startUnicodeValueB, startUnicodeValueC};
                    if (endianness == Endianness::BIGENDIAN) {
                        std::reverse(startUnicodeValues, startUnicodeValues + 3);
                    }
                    unicodeValueRange.startUnicodeValue = static_cast<uint32_t>(CombineBytes(startUnicodeValues, 3));
                    unicodeValueRange.additionalCount = reader.ReadUInt8();
                    format14->defaultUVSTable.unicodeValueRanges[i] = unicodeValueRange;
                }
            }
            if (record.nonDefaultUVSOffset != 0) {
                reader.SetCurrentByte(record.nonDefaultUVSOffset);
                format14->nonDefaultUVSTable.numUVSMappings = reader.ReadUInt32();
                format14->nonDefaultUVSTable.uvsMappings = new Format14::NonDefaultUVsTable::UVsMapping[format14->nonDefaultUVSTable.numUVSMappings];
                for (int i = 0; i < format14->nonDefaultUVSTable.numUVSMappings; ++i) {
                    Format14::NonDefaultUVsTable::UVsMapping uvsMapping;
                    unsigned char unicodeValueA = reader.ReadUChar();
                    unsigned char unicodeValueB = reader.ReadUChar();
                    unsigned char unicodeValueC = reader.ReadUChar();
                    unsigned char unicodeValues[3] = {unicodeValueA, unicodeValueB, unicodeValueC};
                    if (endianness == Endianness::BIGENDIAN) {
                        std::reverse(unicodeValues, unicodeValues + 3);
                    }
                    uvsMapping.unicodeValue = static_cast<uint32_t>(CombineBytes(unicodeValues, 3));
                    uvsMapping.glyphID = reader.ReadUInt16();
                    format14->nonDefaultUVSTable.uvsMappings[i] = uvsMapping;
                }
            }
        }
        return format14;
    }

    Point* ReadPoints(Reader<BIGENDIAN>& reader, SimpleGlyph& simpleGlyph) {
        Point* points = new Point[simpleGlyph.numFlags];

        for (int i = 0; i < simpleGlyph.numFlags; ++i) {
            if (i != 0) {
                points[i].coordinates.x = points[i - 1].coordinates.x;
            } else {
                points[i].coordinates.x = 0;
            }

            uint8_t flag = simpleGlyph.flags[i];
            bool onCurve = FlagBitIsOn(flag, 0);
            points[i].onCurve = onCurve;

            if (FlagBitIsOn(flag, 1)) {
                uint8_t offset = reader.ReadUInt8();
                int16_t sign = FlagBitIsOn(flag, 4) ? 1 : -1;
                points[i].coordinates.x += (FWord)offset * (FWord)sign;
            } else if (!FlagBitIsOn(flag, 4)) {
                points[i].coordinates.x += reader.ReadInt16();
            }
        }

        for (int i = 0; i < simpleGlyph.numFlags; ++i) {
            if (i != 0) {
                points[i].coordinates.y = points[i - 1].coordinates.y;
            } else {
                points[i].coordinates.y = 0;
            }

            uint8_t flag = simpleGlyph.flags[i];

            if (FlagBitIsOn(flag, 2)) {
                uint8_t offset = reader.ReadUInt8();
                int16_t sign = FlagBitIsOn(flag, 5) ? 1 : -1;
                points[i].coordinates.y += (FWord)offset * (FWord)sign;
            } else if (!FlagBitIsOn(flag, 5)) {
                points[i].coordinates.y += reader.ReadInt16();
            }
        }
        return points;
    }
    void ReadSimpleGlyph(SimpleGlyph& simpleGlyph, Reader<BIGENDIAN>& reader, int16_t numberOfContours) {
        simpleGlyph.numberOfContours = numberOfContours;
        if (numberOfContours == 0) { return; }
        FWord xMin = reader.ReadInt16();
        FWord yMin = reader.ReadInt16();
        FWord xMax = reader.ReadInt16();
        FWord yMax = reader.ReadInt16();
        simpleGlyph.min = qbn::vec2<FWord>{xMin, yMin};
        simpleGlyph.max = qbn::vec2<FWord>{xMax, yMax};
        simpleGlyph.endPtsOfContours = new uint16_t[numberOfContours];
        for (int i = 0; i < numberOfContours; ++i) {
            simpleGlyph.endPtsOfContours[i] = reader.ReadUInt16();
        }
        simpleGlyph.instructionLength = reader.ReadUInt16();
        simpleGlyph.instructions = new uint8_t[simpleGlyph.instructionLength];
        for (int i = 0; i < simpleGlyph.instructionLength; ++i) {
            simpleGlyph.instructions[i] = reader.ReadUInt8();
        }
        uint16_t numPoints = simpleGlyph.endPtsOfContours[numberOfContours - 1] + 1;
        simpleGlyph.numFlags = numPoints;
        simpleGlyph.flags = new uint8_t[numPoints];
        for (int i = 0; i < numPoints; ++i) {
            uint8_t flag = reader.ReadUInt8();
            simpleGlyph.flags[i] = flag;

            if (FlagBitIsOn(flag, 3)) {
                uint8_t numRepeats = reader.ReadUInt8();
                uint8_t k = i;
                for (int j = 0; j < numRepeats; ++j) {
                    simpleGlyph.flags[k + j + 1] = flag;
                    ++i;
                }
            }
        }
        simpleGlyph.points = ReadPoints(reader, simpleGlyph);
    }
    void ReadComponentGlyph(ComponentGlyph& componentGlyph, Reader<BIGENDIAN>& reader) {
        componentGlyph.flags = reader.ReadUInt16();
        componentGlyph.glyphIndex = reader.ReadUInt16();
        int16_t xOffset = FlagBitIsOn(componentGlyph.flags, 0) ? reader.ReadInt16() : reader.ReadInt8();
        int16_t yOffset = FlagBitIsOn(componentGlyph.flags, 0) ? reader.ReadInt16() : reader.ReadInt8();
        componentGlyph.offset = qbn::vec2<int16_t>{xOffset, yOffset};
        if (FlagBitIsOn(componentGlyph.flags, 3)) {
            Transform1* transform = new Transform1();
            transform->transformType = 1;
            transform->scale = reader.ReadFixedPoint(2, 14);
            componentGlyph.transform = transform;
        } else if (FlagBitIsOn(componentGlyph.flags, 6)) {
            Transform2* transform = new Transform2();
            transform->transformType = 2;
            double scaleX = reader.ReadFixedPoint(2, 14);
            double scaleY = reader.ReadFixedPoint(2, 14);
            transform->scale = {scaleX, scaleY};
            componentGlyph.transform = transform;
        } else if (FlagBitIsOn(componentGlyph.flags, 7)) {
            Transform3* transform = new Transform3();
            transform->transformType = 2;
            double scaleXX = reader.ReadFixedPoint(2, 14);
            double scaleXY = reader.ReadFixedPoint(2, 14);
            double scaleYX = reader.ReadFixedPoint(2, 14);
            double scaleYY = reader.ReadFixedPoint(2, 14);
            transform->scale = {{scaleXX, scaleYX}, {scaleXY, scaleYY}};
            componentGlyph.transform = transform;
        } else {
            Transform0* transform = new Transform0();
            transform->transformType = 0;
            transform->scale = 1;
            componentGlyph.transform = transform;
        }
    }
    void ReadCompoundGlyph(CompoundGlyph& compoundGlyph, Reader<BIGENDIAN>& reader, int16_t numberOfContours) {
        compoundGlyph.numberOfContours = numberOfContours;
        FWord xMin = reader.ReadInt16();
        FWord yMin = reader.ReadInt16();
        FWord xMax = reader.ReadInt16();
        FWord yMax = reader.ReadInt16();
        compoundGlyph.min = qbn::vec2<FWord>{xMin, yMin};
        compoundGlyph.max = qbn::vec2<FWord>{xMax, yMax};

        int numComponentGlyphs = 0;
        unsigned long long position = reader.GetPosition();
        while (true) {
            ComponentGlyph componentGlyph{};
            ReadComponentGlyph(componentGlyph, reader);
            ++numComponentGlyphs;
            if (!FlagBitIsOn(componentGlyph.flags, 5)) {
                break;
            }
        }
        compoundGlyph.numComponentGlyphs = numComponentGlyphs;
        compoundGlyph.component_glyphs = new ComponentGlyph[numComponentGlyphs];

        reader.SetCurrentByte(position);
        for (int i = 0; i < numComponentGlyphs; ++i) {
            ReadComponentGlyph(compoundGlyph.component_glyphs[i], reader);
        }
    }
    TTFGlyph* ReadGlyph(Reader<BIGENDIAN>& reader, uint32_t location, int index) {
        reader.SetCurrentByte(location);
        int16_t numberOfContours = reader.ReadInt16();
        if (numberOfContours < 0) {
            CompoundGlyph* compoundGlyph = new CompoundGlyph();
            ReadCompoundGlyph(*compoundGlyph, reader, numberOfContours);
            compoundGlyph->glyphType = 1;
            return compoundGlyph;
        } else {
            SimpleGlyph* simpleGlyph = new SimpleGlyph();
            ReadSimpleGlyph(*simpleGlyph, reader, numberOfContours);
            simpleGlyph->glyphType = 0;
            return simpleGlyph;
        }
    }

    // *********************************************************************************************
    //                                       KERN FUNCTIONS
    // *********************************************************************************************

    KERN::Subtable0* GetSubtable0(Reader<BIGENDIAN>& reader) {
        KERN::Subtable0* subtable0 = new KERN::Subtable0();

        subtable0->nPairs = reader.ReadUInt16();
        subtable0->searchRange = reader.ReadUInt16();
        subtable0->entrySelector = reader.ReadUInt16();
        subtable0->rangeShift = reader.ReadUInt16();

        subtable0->pairs = new KERN::Subtable0::Pair[subtable0->nPairs];

        for (int i = 0; i < subtable0->nPairs; ++i) {
            KERN::Subtable0::Pair pair;
            pair.left = reader.ReadUInt16();
            pair.right = reader.ReadUInt16();
            pair.value = reader.ReadInt16();

            subtable0->pairs[i] = pair;
        }

        return subtable0;
    }

    // *********************************************************************************************
    //                                       MORE FUNCTIONS
    // *********************************************************************************************

    // Sets the offsets of all the tables
    std::vector<std::string> GetTableOffsets(TrueTypeFont& ttf, Reader<BIGENDIAN>& reader) {
        std::vector<std::string> tags;
        reader.ResetCurrentByte();
        reader.SkipBytes(4);
        uint16_t numTables = reader.ReadUInt16();
        reader.SkipBytes(6);
        for (int i = 0; i < numTables; ++i) {
            std::string tag = reader.ReadString(4);
            tags.push_back(tag);
            reader.SkipBytes(4);
            uint32_t offset = reader.ReadUInt32();
            reader.SkipBytes(4);
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
        reader.ResetCurrentByte();
        return tags;
    }

    void TrueTypeFont::Init(Reader<BIGENDIAN>& reader) {
        std::vector<std::string> tags = GetTableOffsets(*this, reader);

        // Required
        InitCMAP(reader);
        InitHEAD(reader);
        InitMAXP(reader);
        InitLOCA(reader);
        InitGLYF(reader);
        InitHHEA(reader);
        InitHMTX(reader);
        InitNAME(reader);
        InitPOST(reader);

        // Optional
        if (HasTag(tags, "kern")) {
            InitKERN(reader);
        }

        reader.ClearBuffer();
    }

    // *********************************************************************************************
    //                            TRUETYPEFONT PRIVATE STRUCT DEFINITIONS
    // *********************************************************************************************

    // Initializes the cmap table
    void TrueTypeFont::InitCMAP(Reader<BIGENDIAN>& reader) {
        reader.SetCurrentByte(cmap.offset);
        cmap.version = reader.ReadUInt16();
        cmap.numberSubtables = reader.ReadUInt16();

        std::vector<Subtable> validSubtables;
        for (int i = 0; i < cmap.numberSubtables; ++i) {
            Subtable subtable;
            subtable.platformID = reader.ReadUInt16();
            subtable.platformSpecificID = reader.ReadUInt16();
            subtable.offset = reader.ReadUInt32();

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

        cmap.numberSubtables = validSubtables.size();
        cmap.subtables = new Subtable[validSubtables.size()];

        int i = 0;
        for (Subtable& validSubtable : validSubtables) {
            Subtable subtable;
            subtable.format = validSubtable.format;
            subtable.formatID = validSubtable.formatID;
            subtable.offset = validSubtable.offset;
            subtable.platformID = validSubtable.platformID;
            subtable.platformSpecificID = validSubtable.platformSpecificID;
            
            reader.SetCurrentByte(cmap.offset + subtable.offset);
            subtable.formatID = reader.ReadUInt16();
            switch (subtable.formatID) {
                case 0: subtable.format = GetFormat0(reader); break;
                case 2: subtable.format = GetFormat2(reader); break;
                case 4: subtable.format = GetFormat4(reader); break;
                case 6: subtable.format = GetFormat6(reader); break;
                case 8: subtable.format = GetFormat8(reader); break;
                case 10: subtable.format = GetFormat10(reader); break;
                case 12: subtable.format = GetFormat12(reader); break;
                case 13: subtable.format = GetFormat13(reader); break;
                case 14: subtable.format = GetFormat14(reader); break;
                default: std::runtime_error("Invalid format for subtable.");
            }
            cmap.subtables[i++] = subtable;
        }
    }
    // Initializes the head table
    void TrueTypeFont::InitHEAD(Reader<BIGENDIAN>& reader) {
        reader.SetCurrentByte(head.offset);
        head.version = reader.ReadInt32();
        head.fontRevision = reader.ReadInt32();
        head.checkSumAdjustment = reader.ReadUInt32();
        head.magicNumber = reader.ReadUInt32();
        head.flags = reader.ReadUInt16();
        head.unitsPerEm = reader.ReadUInt16();
        head.created = reader.ReadInt64();
        head.modified = reader.ReadInt64();
        FWord xMin = reader.ReadInt16();
        FWord yMin = reader.ReadInt16();
        FWord xMax = reader.ReadInt16();
        FWord yMax = reader.ReadInt16();
        head.min = qbn::vec2<FWord>{xMin, yMin};
        head.max = qbn::vec2<FWord>{xMax, yMax};
        head.macStyle = reader.ReadUInt16();
        head.lowestRecPPEM = reader.ReadUInt16();
        head.fontDirectionHint = reader.ReadInt16();
        head.indexToLocFormat = reader.ReadInt16();
        head.glyphDataFormat = reader.ReadInt16();
    }
    // Initializes the maxp table
    void TrueTypeFont::InitMAXP(Reader<BIGENDIAN>& reader) {
        reader.SetCurrentByte(maxp.offset);
        maxp.version = reader.ReadInt32();
        maxp.numGlyphs = reader.ReadUInt16();
        maxp.maxPoints = reader.ReadUInt16();
        maxp.maxContours = reader.ReadUInt16();
        maxp.maxComponentPoints = reader.ReadUInt16();
        maxp.maxComponentContours = reader.ReadUInt16();
        maxp.maxZones = reader.ReadUInt16();
        maxp.maxTwilightPoints = reader.ReadUInt16();
        maxp.maxStorage = reader.ReadUInt16();
        maxp.maxFunctionDefs = reader.ReadUInt16();
        maxp.maxInstructionDefs = reader.ReadUInt16();
        maxp.maxStackElements = reader.ReadUInt16();
        maxp.maxSizeOfInstructions = reader.ReadUInt16();
        maxp.maxComponentElements = reader.ReadUInt16();
        maxp.maxComponentDepth = reader.ReadUInt16();
    }
    // Initializes the loca table
    void TrueTypeFont::InitLOCA(Reader<BIGENDIAN>& reader) {
        reader.SetCurrentByte(loca.offset);
        loca.glyphOffsets = new uint32_t[maxp.numGlyphs];
        for (int i = 0; i < maxp.numGlyphs; ++i) {
            uint32_t glyphOffset = (head.indexToLocFormat == 0) ? reader.ReadUInt16() * 2u : reader.ReadUInt32();
            loca.glyphOffsets[i] = glyphOffset; // Offset from ttf.glyf.offset
        }
    }
    // Initializes the glyf table
    void TrueTypeFont::InitGLYF(Reader<BIGENDIAN>& reader) {
        glyf.glyphs = new TTFGlyph*[maxp.numGlyphs];
        glyf.numGlyphs = maxp.numGlyphs;
        for (int i = 0; i < maxp.numGlyphs; ++i) {
            TTFGlyph* glyph = ReadGlyph(reader, loca.glyphOffsets[i] + glyf.offset, i);
            glyf.glyphs[i] = glyph;
        }
    }
    // Initializes the hhea table
    void TrueTypeFont::InitHHEA(Reader<BIGENDIAN>& reader) {
        reader.SetCurrentByte(hhea.offset);
        hhea.version = reader.ReadInt32();
        hhea.ascent = reader.ReadInt16();
        hhea.descent = reader.ReadInt16();
        hhea.lineGap = reader.ReadInt16();
        hhea.advanceWidthMax = reader.ReadUInt16();
        hhea.minLeftSideBearing = reader.ReadInt16();
        hhea.minRightSideBearing = reader.ReadInt16();
        hhea.xMaxExtent = reader.ReadInt16();
        hhea.caretSlopeRise = reader.ReadInt16();
        hhea.caretSlopeRun = reader.ReadInt16();
        hhea.caretOffset = reader.ReadInt16();
        hhea.reserved = reader.ReadInt64();
        hhea.metricDataFormat = reader.ReadInt16();
        hhea.numOfLongHorMetrics = reader.ReadUInt16();
    }
    // Initializes the hmtx table
    void TrueTypeFont::InitHMTX(Reader<BIGENDIAN>& reader) {
        reader.SetCurrentByte(hmtx.offset);
        hmtx.hMetrics = new HMTX::LongHorMetric[hhea.numOfLongHorMetrics];
        for (int i = 0; i < hhea.numOfLongHorMetrics; ++i) {
            HMTX::LongHorMetric longHorMetric;
            longHorMetric.advanceWidth = reader.ReadUInt16();
            longHorMetric.leftSideBearing = reader.ReadInt16();
            hmtx.hMetrics[i] = longHorMetric;
        }
        uint16_t numBearings = maxp.numGlyphs - hhea.numOfLongHorMetrics;
        if (numBearings > 0) {
            hmtx.leftSideBearing = new FWord[numBearings];
            for (int i = 0; i < numBearings; ++i) {
                hmtx.leftSideBearing[i] = reader.ReadInt16();
            }
        }
    }
    // Initializes the name table
    void TrueTypeFont::InitNAME(Reader<BIGENDIAN>& reader) {
        reader.SetCurrentByte(name.offset);
        name.format = reader.ReadUInt16();
        name.count = reader.ReadUInt16();
        name.stringOffset = reader.ReadUInt16();

        name.nameRecords = new NAME::NameRecord[name.count];
        for (int i = 0; i < name.count; ++i) {
            NAME::NameRecord nameRecord;
            nameRecord.platformID = reader.ReadUInt16();
            nameRecord.platformSpecificID = reader.ReadUInt16();
            nameRecord.languageID = reader.ReadUInt16();
            nameRecord.nameID = reader.ReadUInt16();
            nameRecord.length = reader.ReadUInt16();
            nameRecord.offset = reader.ReadUInt16();
            name.nameRecords[i] = nameRecord;
        }
        for (int i = 0; i < name.count; ++i) {
            NAME::NameRecord& nameRecord = name.nameRecords[i];
            reader.SetCurrentByte(name.offset + name.stringOffset + nameRecord.offset);
            nameRecord.name = reader.ReadString(nameRecord.length);
        }
    }
    // Initializes the post table
    void TrueTypeFont::InitPOST(Reader<BIGENDIAN>& reader) {
        reader.SetCurrentByte(post.offset);
        post.format = reader.ReadInt32();
        post.italicAngle = reader.ReadInt32();
        post.underlinePosition = reader.ReadInt16();
        post.underlineThickness = reader.ReadInt16();
        post.isFixedPitch = reader.ReadUInt32();
        post.minMemType42 = reader.ReadUInt32();
        post.maxMemType42 = reader.ReadUInt32();
        post.minMemType1 = reader.ReadUInt32();
        post.maxMemType1 = reader.ReadUInt32();
    }

    // Initializes the kern table
    void TrueTypeFont::InitKERN(Reader<BIGENDIAN>& reader) {
        reader.SetCurrentByte(kern.offset);
        kern.version = reader.ReadUInt16();
        if (kern.version == 0) {
            kern.nTables = reader.ReadUInt16();
        } else {
            reader.SetCurrentByte(kern.offset);
            kern.version = reader.ReadInt32();
            kern.nTables = reader.ReadUInt32();
        }

        kern.subtables = new KERN::Subtable*[kern.nTables];

        for (int i = 0; i < kern.nTables; ++i) {
            uint32_t length = reader.ReadUInt32();
            uint16_t coverage = reader.ReadUInt16();
            uint16_t tupleIndex = reader.ReadUInt16();

            bool kernVertical = FlagBitIsOn(coverage, 0);
            bool kernCrossStream = FlagBitIsOn(coverage, 1);
            bool kernVariation = FlagBitIsOn(coverage, 2);

            uint8_t format = (coverage << 8);

            if (format == 0) {
                KERN::Subtable0* subtable0 = GetSubtable0(reader);
                subtable0->length = length;
                subtable0->tupleIndex = tupleIndex;
                subtable0->format = format;
                subtable0->kernVertical = kernVertical;
                subtable0->kernCrossStream = kernCrossStream;
                subtable0->kernVariation = kernVariation;
                kern.subtables[i] = subtable0;
            }
        }
    }

}