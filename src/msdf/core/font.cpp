#include <msdf/core/font.hpp>

// ttf
#include <truetypefont/truetypefont.hpp>

// qarbon
#include <qarbon.hpp>

// msdf
#include <msdf/core/shape/edge.hpp>
#include <msdf/core/shape/contour.hpp>
#include <msdf/core/glyph.hpp>
// #include <msdf/core/resolve_shape_geometry.hpp>

namespace qtxt {
    using vec2 = qbn::vec2<double>;

    struct TempPoint {
        vec2 coords;
        bool onCurve;
    };

    inline static std::map<std::pair<int, int>, int> CreateKerning(TrueTypeFont& ttf) {
        std::map<std::pair<int, int>, int> kerning;

        for (int i = 0; i < ttf.kern.nTables; ++i) {
            KERN::Subtable* subtable = ttf.kern.subtables[i];
            
            if (subtable->format == 0) {
                KERN::Subtable0* subtable0 = (KERN::Subtable0*)subtable;
                for (int j = 0; j < subtable0->nPairs; ++j) {
                    KERN::Subtable0::Pair& pair = subtable0->pairs[j];

                    kerning[{pair.left, pair.right}] = pair.value;
                }
            }
        }

        return kerning;
    }
    inline static std::map<uint32_t, uint32_t> createMapping(Format0* format0) { // Not tested
        std::map<uint32_t, uint32_t> mapping;
        for (int i = 0; i < format0->glyphIndexArraySize; ++i) {
            mapping[i] = format0->glyphIndexArray[i];
        }
        return mapping;
    }
    inline static std::map<uint32_t, uint32_t> createMapping(Format2* format2) { // Not tested
        std::map<uint32_t, uint32_t> mapping;
        for (int i = 0; i < format2->subHeaderKeysSize; ++i) {
            const Format2::Subheader& sh = format2->subHeaders[format2->subHeaderKeys[i] / 8];
            for (uint16_t j = 0; j < sh.entryCount; ++j) {
                uint32_t charCode = sh.firstCode + j;
                uint32_t glyphIndex;
                if (sh.idRangeOffset == 0) {
                    glyphIndex = (sh.idDelta + format2->glyphIndexArray[sh.idRangeOffset / 2 + j]) % 65536;
                } else {
                    glyphIndex = (sh.idDelta + charCode) % 65536;
                }
                mapping[charCode] = glyphIndex;
            }
        }
        return mapping;
    }
    inline static std::map<uint32_t, uint32_t> createMapping(Format4* format4) {
        std::map<uint32_t, uint32_t> mapping;
        uint16_t segCount = format4->segCountX2 / 2;
        for (int i = 0; i < segCount; i++) {
            for (uint32_t charCode = format4->startCode[i]; charCode <= format4->endCode[i]; charCode++) {
                uint32_t glyphIndex;
                if (format4->idRangeOffset[i] == 0) {
                    glyphIndex = (charCode + format4->idDelta[i]) % 65536;
                } else {
                    int glyphOffset = format4->idRangeOffset[i] / 2 + (charCode - format4->startCode[i]) + i - segCount;
                    glyphIndex = format4->glyphIndexArray[glyphOffset];
                    if (glyphIndex != 0) {
                        glyphIndex = (glyphIndex + format4->idDelta[i]) % 65536;
                    }
                }
                mapping[charCode] = glyphIndex;
            }
        }
        return mapping;
    }
    inline static std::map<uint32_t, uint32_t> createMapping(Format6* format6) { // Not tested
        std::map<uint32_t, uint32_t> mapping;
        for (uint32_t i = 0; i < format6->entryCount; ++i) {
            mapping[format6->firstCode + i] = format6->glyphIndexArray[i];
        }
        return mapping;
    }
    inline static std::map<uint32_t, uint32_t> createMapping(Format8* format8) { // Not tested
        std::map<uint32_t, uint32_t> mapping;
        for (uint32_t i = 0; i < format8->nGroups; ++i) {
            Format8::Group& group = format8->groups[i];
            for (uint32_t charCode = group.startCharCode; charCode <= group.endCharCode; ++charCode) {
                mapping[charCode] = group.startGlyphCode + (charCode - group.startCharCode);
            }
        }
        return mapping;
    }
    inline static std::map<uint32_t, uint32_t> createMapping(Format10* format10) { // Not tested
        std::map<uint32_t, uint32_t> mapping;
        for (uint32_t i = 0; i < format10->numChars; ++i) {
            mapping[format10->startCharCode + i] = format10->glyphs[i];
        }
        return mapping;
    }
    inline static std::map<uint32_t, uint32_t> createMapping(Format12* format12) { // Working
        std::map<uint32_t, uint32_t> mapping;
        for (uint32_t i = 0; i < format12->nGroups; ++i) {
            Format12::Group& group = format12->groups[i];
            for (uint32_t charCode = group.startCharCode; charCode <= group.endCharCode; ++charCode) {
                mapping[charCode] = group.startGlyphCode + (charCode - group.startCharCode);
            }
        }
        return mapping;
    }
    inline static std::map<uint32_t, uint32_t> createMapping(Format13* format13) { // Not tested
        std::map<uint32_t, uint32_t> mapping;
        for (uint32_t i = 0; i < format13->nGroups; ++i) {
            Format13::Group& group = format13->groups[i];
            for (uint32_t charCode = group.startCharCode; charCode <= group.endCharCode; ++charCode) {
                mapping[charCode] = group.startGlyphCode;
            }
        }
        return mapping;
    }
    inline static std::map<uint32_t, uint32_t> createMapping(Format14* format14) { // Not tested
        std::map<uint32_t, uint32_t> mapping;
        for (uint32_t i = 0; i < format14->defaultUVSTable.numUnicodeValueRanges; ++i) {
            Format14::DefaultUVsTable::UnicodeValueRange& range = format14->defaultUVSTable.unicodeValueRanges[i];
            for (uint32_t j = 0; j <= range.additionalCount; ++j) {
                mapping[range.startUnicodeValue + j] = 0;
            }
        }
        for (uint32_t i = 0; format14->nonDefaultUVSTable.numUVSMappings; ++i) {
            Format14::NonDefaultUVsTable::UVsMapping& fmapping = format14->nonDefaultUVSTable.uvsMappings[i];
            mapping[fmapping.unicodeValue] = fmapping.glyphID;
        }
        return mapping;
    }

    inline static std::vector<vec2> createContourWithImpliedPoints(const Point* points, uint32_t numPoints) {
        uint8_t pointOffset;
        for (pointOffset = 0; pointOffset < numPoints; pointOffset++) {
            if (points[pointOffset].onCurve == true) {
                break;
            }
        }

        std::vector<vec2> new_points;
        for (int i = 0; i < numPoints; i++) {
            Point currentPoint = points[(i + pointOffset + 0) % numPoints];
            Point nextPoint = points[(i + pointOffset + 1) % numPoints];
            vec2 new_point = qbn::vectype_cast<double>(currentPoint.coordinates);
            new_points.push_back(new_point);

            if (currentPoint.onCurve == nextPoint.onCurve && i < numPoints) {
                vec2 midpoint = {(currentPoint.coordinates.x + nextPoint.coordinates.x) / 2.0f, (currentPoint.coordinates.y + nextPoint.coordinates.y) / 2.0f};
                new_points.push_back(midpoint);
            }
        }
        return new_points;
    }

    inline static std::vector<TempPoint> createContourWithImpliedPointsSimple(const Point* points, uint32_t numPoints) {
        uint8_t pointOffset;
        for (pointOffset = 0; pointOffset < numPoints; pointOffset++) {
            if (points[pointOffset].onCurve == true) {
                break;
            }
        }

        std::vector<TempPoint> new_points;
        for (int i = 0; i < numPoints; i++) {
            Point currentPoint = points[(i + pointOffset + 0) % numPoints];
            Point nextPoint = points[(i + pointOffset + 1) % numPoints];
            vec2 new_point = qbn::vectype_cast<double>(currentPoint.coordinates);
            new_points.push_back({new_point, currentPoint.onCurve});

            if (currentPoint.onCurve == nextPoint.onCurve && i < numPoints) {
                vec2 midpoint = {(currentPoint.coordinates.x + nextPoint.coordinates.x) / 2.0f, (currentPoint.coordinates.y + nextPoint.coordinates.y) / 2.0f};
                new_points.push_back({midpoint, !currentPoint.onCurve});
            }
        }
        return new_points;
    }

    Shape createShapeFromSimpleGlyph(SimpleGlyph* simple_glyph, int j, int debug = -1) {
        Shape shape;
        uint16_t startIndex = 0;
        for (int i = 0; i < simple_glyph->numberOfContours; ++i) {
            uint16_t endIndex = simple_glyph->endPtsOfContours[i] + 1;
            Contour contour;
            std::vector<Point> temp_points(&simple_glyph->points[startIndex], &simple_glyph->points[endIndex]);
            std::vector<TempPoint> points = createContourWithImpliedPointsSimple(temp_points.data(), temp_points.size());
            uint16_t numPoints = points.size();
            
            for (uint16_t j = 0; j < numPoints; ++j) {
                const TempPoint& p0 = points[j];
                const TempPoint& p1 = points[(j + 1) % numPoints];

                if (p1.onCurve) {
                    vec2 start = p0.coords;
                    vec2 end = p1.coords;
                    contour.add(Edge(start, end));
                    continue;
                }

                const TempPoint& p2 = points[(j + 2) % numPoints];

                if (p2.onCurve) {
                    vec2 start = p0.coords;
                    vec2 control = p1.coords;
                    vec2 end = p2.coords;
                    contour.add(Edge(start, control, end));
                    ++j;
                    continue;
                }

                const TempPoint& p3 = points[(j + 3) % numPoints];
                vec2 start = p0.coords;
                vec2 control1 = p1.coords;
                vec2 control2 = p2.coords;
                vec2 end = p3.coords;
                contour.add(Edge(start, control1, control2, end));
                j += 2;
            }
            shape.add(contour);
            startIndex = endIndex;
        }
        return shape;
    }

    vec2 applyTransform(vec2 v, ComponentGlyph::Transform* transform, int type, qbn::vec2<int16_t> offset) {
        vec2 o = qbn::vectype_cast<double>(offset);
        if (type == 0) {
            return v + o;
        } else if (type == 1) {
            ComponentGlyph::Transform1& transform1 = *((ComponentGlyph::Transform1*)transform);
            return transform1.scale * v + o;
        } else if (type == 2) {
            ComponentGlyph::Transform2& transform2 = *((ComponentGlyph::Transform2*)transform);
            return transform2.scale * v + o;
        } else if (type == 3) {
            ComponentGlyph::Transform3& transform3 = *((ComponentGlyph::Transform3*)transform);
            return transform3.scale * v + o;
        }

        return vec2(0.0, 0.0);
    }

    Shape createShapeFromCompoundGlyph(TTFGlyph** glyphs, uint32_t index) {
        Shape shape;
        CompoundGlyph* compound_glyph = (CompoundGlyph*)(glyphs[index]);
        for (int i = 0; i < compound_glyph->numComponentGlyphs; ++i) {
            const ComponentGlyph& component_glyph = compound_glyph->component_glyphs[i];
            const SimpleGlyph* simple_glyph = (SimpleGlyph*)(glyphs[component_glyph.glyphIndex]);
            uint16_t startIndex = 0;
            for (int j = 0; j < simple_glyph->numberOfContours; ++j) {
                uint16_t endIndex = simple_glyph->endPtsOfContours[j] + 1;
                Contour contour;
                std::vector<Point> temp_points(&simple_glyph->points[startIndex], &simple_glyph->points[endIndex]);
                std::vector<TempPoint> points = createContourWithImpliedPointsSimple(temp_points.data(), temp_points.size());
                uint16_t numPoints = points.size();

                for (uint16_t k = 0; k < numPoints; ++k) {
                    const TempPoint& p0 = points[k];
                    const TempPoint& p1 = points[(k + 1) % numPoints];

                    if (p1.onCurve) {
                        vec2 start = applyTransform(p0.coords, component_glyph.transform, (int)component_glyph.transform->transformType, component_glyph.offset);
                        vec2 end = applyTransform(p1.coords, component_glyph.transform, (int)component_glyph.transform->transformType, component_glyph.offset);
                        contour.add(Edge(start, end));
                        continue;
                    }

                    const TempPoint& p2 = points[(k + 2) % numPoints];

                    if (p2.onCurve) {
                        vec2 start = applyTransform(p0.coords, component_glyph.transform, (int)component_glyph.transform->transformType, component_glyph.offset);
                        vec2 control = applyTransform(p1.coords, component_glyph.transform, (int)component_glyph.transform->transformType, component_glyph.offset);
                        vec2 end = applyTransform(p2.coords, component_glyph.transform, (int)component_glyph.transform->transformType, component_glyph.offset);
                        contour.add(Edge(start, control, end));
                        ++k;
                        continue;
                    }

                    const TempPoint& p3 = points[(k + 3) % numPoints];
                    vec2 start = applyTransform(p0.coords, component_glyph.transform, (int)component_glyph.transform->transformType, component_glyph.offset);
                    vec2 control1 = applyTransform(p1.coords, component_glyph.transform, (int)component_glyph.transform->transformType, component_glyph.offset);
                    vec2 control2 = applyTransform(p2.coords, component_glyph.transform, (int)component_glyph.transform->transformType, component_glyph.offset);
                    vec2 end = applyTransform(p3.coords, component_glyph.transform, (int)component_glyph.transform->transformType, component_glyph.offset);
                    contour.add(Edge(start, control1, control2, end));
                    k += 2;
                }
                shape.add(contour);
                startIndex = endIndex;
            }
        }
        return shape;
    }

    GlyphMetrics GetMetrics(TrueTypeFont& ttf, uint32_t index) {
        GlyphMetrics metrics;
        if (index < ttf.hhea.numOfLongHorMetrics) {
            metrics.advanceWidth = ttf.hmtx.hMetrics[index].advanceWidth;
            metrics.leftSideBearing = ttf.hmtx.hMetrics[index].leftSideBearing;
        } else {
            metrics.advanceWidth = ttf.hmtx.hMetrics[ttf.hhea.numOfLongHorMetrics - 1].advanceWidth;
            metrics.leftSideBearing = ttf.hmtx.leftSideBearing[index - ttf.hhea.numOfLongHorMetrics];
        }

        return metrics;
    }

    Font::Font(const std::string& path) {
        TrueTypeFont ttf = TrueTypeFont(path);
        metrics.emSize = double(ttf.head.unitsPerEm);
        metrics.ascenderY = double(ttf.hhea.ascent);
        metrics.descenderY = double(ttf.hhea.descent);
        metrics.lineHeight = double(ttf.hhea.ascent - ttf.hhea.descent + ttf.hhea.lineGap);
        metrics.underlineY = double(ttf.post.underlinePosition);
        metrics.underlineThickness = double(ttf.post.underlineThickness);

        for (int i = 0; i < ttf.glyf.numGlyphs; ++i) {
            Shape shape = ttf.glyf.glyphs[i]->glyphType ? createShapeFromCompoundGlyph(ttf.glyf.glyphs, i) : createShapeFromSimpleGlyph(dynamic_cast<SimpleGlyph*>(ttf.glyf.glyphs[i]), i, 816);
            GlyphMetrics glyphMetrics = GetMetrics(ttf, i);
            // ResolveShapeGeometry(shape);
            glyphs.push_back(Glyph(shape, glyphMetrics, i));
        }
        if (ttf.cmap.numberSubtables > 0) {
            Subtable& subtable = ttf.cmap.subtables[0];
            switch (subtable.formatID) {
                case 0: mapping = createMapping(static_cast<Format0*>(subtable.format)); break;
                case 2: mapping = createMapping(static_cast<Format2*>(subtable.format)); break;
                case 4: mapping = createMapping(static_cast<Format4*>(subtable.format)); break;
                case 6: mapping = createMapping(static_cast<Format6*>(subtable.format)); break;
                case 8: mapping = createMapping(static_cast<Format8*>(subtable.format)); break;
                case 10: mapping = createMapping(static_cast<Format10*>(subtable.format)); break;
                case 12: mapping = createMapping(static_cast<Format12*>(subtable.format)); break;
                case 13: mapping = createMapping(static_cast<Format13*>(subtable.format)); break;
                case 14: mapping = createMapping(static_cast<Format14*>(subtable.format)); break;
            }
        } else {
            throw std::runtime_error("Invalid number of subtables.");
        }
        for (int i = 0; i < ttf.name.count; ++i) {
            NAME::NameRecord& nameRecord = ttf.name.nameRecords[i];
            if (nameRecord.nameID == 4) {
                name = nameRecord.name;
                break;
            }
        }
        if (ttf.kern.offset != 0) {
            kerning = CreateKerning(ttf);
        }
    }

    Font::~Font() {
        glyphs.clear();
    }

    Glyph& Font::getGlyph(uint32_t unicode) {
        auto it = mapping.find(unicode);
        if (it == mapping.end()) {
            throw std::runtime_error("Could not find glyph in map.");
        }
        return glyphs[it->second];
    }

    std::vector<Glyph>& Font::getGlyphs() {
        return glyphs;
    }

    const FontMetrics& Font::getFontMetrics() const {
        return metrics;
    }
    
    double Font::getKerning(uint32_t codepointA, uint32_t codepointB, double geometryScale) {
        return double(kerning[{codepointA, codepointB}]) * geometryScale;
    }

    double Font::getAdvance(uint32_t codepointA, uint32_t codepointB, double geometryScale) {
        return getGlyph(codepointA).getAdvance() * geometryScale + getKerning(codepointA, codepointB, geometryScale);
    }

    const std::map<std::pair<int, int>, int>& Font::getKerningMap() const {
        return kerning;
    }
}