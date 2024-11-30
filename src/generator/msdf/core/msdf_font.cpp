#include <generator/msdf/core/msdf_font.hpp>

// std
#include <iostream>

// reader
#include <reader/ttf_reader.hpp>

// qarbon
#include <qarbon.hpp>

// msdf
#include <generator/msdf/core/edge_holder.hpp>
#include <generator/msdf/core/contour.hpp>
#include <generator/msdf/core/glyph.hpp>
#include <generator/msdf/core/resolve_shape_geometry.hpp>

using SIMPLEGLYPH = qtxt::GLYF::SIMPLEGLYPH;
using COMPOUNDGLYPH = qtxt::GLYF::COMPOUNDGLYPH;
using COMPONENTGLYPH = COMPOUNDGLYPH::COMPONENTGLYPH;
using TRANSFORM0 = COMPONENTGLYPH::TRANSFORM0;
using TRANSFORM1 = COMPONENTGLYPH::TRANSFORM1;
using TRANSFORM2 = COMPONENTGLYPH::TRANSFORM2;
using TRANSFORM3 = COMPONENTGLYPH::TRANSFORM3;
using GLYPH = qtxt::GLYF::GLYPH;

using FORMAT = qtxt::CMAP::FORMAT;
using FORMAT0 = qtxt::CMAP::FORMAT0;
using FORMAT2 = qtxt::CMAP::FORMAT2;
using FORMAT4 = qtxt::CMAP::FORMAT4;
using FORMAT6 = qtxt::CMAP::FORMAT6;
using FORMAT8 = qtxt::CMAP::FORMAT8;
using FORMAT10 = qtxt::CMAP::FORMAT10;
using FORMAT12 = qtxt::CMAP::FORMAT12;
using FORMAT13 = qtxt::CMAP::FORMAT13;
using FORMAT14 = qtxt::CMAP::FORMAT14;
using SUBTABLE = qtxt::CMAP::SUBTABLE;

using vec2 = qbn::vec2<double>;

namespace qtxt {
    struct TempPoint {
        vec2 coords;
        bool onCurve;
    };

    inline static std::map<std::pair<int, int>, int> CreateKerning(TTF& ttf) {
        std::map<std::pair<int, int>, int> kerning;

        for (int i = 0; i < ttf.kern.nTables; ++i) {
            KERN::SUBTABLE* subtable = ttf.kern.subtables[i];
            
            if (subtable->format == 0) {
                KERN::SUBTABLE0* subtable0 = (KERN::SUBTABLE0*)subtable;
                for (int j = 0; j < subtable0->nPairs; ++j) {
                    KERN::SUBTABLE0::PAIR& pair = subtable0->pairs[j];

                    kerning[{pair.left, pair.right}] = pair.value;
                }
            }
        }

        return kerning;
    }
    inline static std::map<uint32_t, uint32_t> CreateMapping(FORMAT0* format0) { // Not tested
        std::map<uint32_t, uint32_t> mapping;
        for (int i = 0; i < format0->glyphIndexArraySize; ++i) {
            mapping[i] = format0->glyphIndexArray[i];
        }
        return mapping;
    }
    inline static std::map<uint32_t, uint32_t> CreateMapping(FORMAT2* format2) { // Not tested
        std::map<uint32_t, uint32_t> mapping;
        for (int i = 0; i < format2->subHeaderKeysSize; ++i) {
            const FORMAT2::SUBHEADER& sh = format2->subHeaders[format2->subHeaderKeys[i] / 8];
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
    inline static std::map<uint32_t, uint32_t> CreateMapping(FORMAT4* format4) {
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
    inline static std::map<uint32_t, uint32_t> CreateMapping(FORMAT6* format6) { // Not tested
        std::map<uint32_t, uint32_t> mapping;
        for (uint32_t i = 0; i < format6->entryCount; ++i) {
            mapping[format6->firstCode + i] = format6->glyphIndexArray[i];
        }
        return mapping;
    }
    inline static std::map<uint32_t, uint32_t> CreateMapping(FORMAT8* format8) { // Not tested
        std::map<uint32_t, uint32_t> mapping;
        for (uint32_t i = 0; i < format8->nGroups; ++i) {
            FORMAT8::GROUP& group = format8->groups[i];
            for (uint32_t charCode = group.startCharCode; charCode <= group.endCharCode; ++charCode) {
                mapping[charCode] = group.startGlyphCode + (charCode - group.startCharCode);
            }
        }
        return mapping;
    }
    inline static std::map<uint32_t, uint32_t> CreateMapping(FORMAT10* format10) { // Not tested
        std::map<uint32_t, uint32_t> mapping;
        for (uint32_t i = 0; i < format10->numChars; ++i) {
            mapping[format10->startCharCode + i] = format10->glyphs[i];
        }
        return mapping;
    }
    inline static std::map<uint32_t, uint32_t> CreateMapping(FORMAT12* format12) { // Working
        std::map<uint32_t, uint32_t> mapping;
        for (uint32_t i = 0; i < format12->nGroups; ++i) {
            FORMAT12::GROUP& group = format12->groups[i];
            for (uint32_t charCode = group.startCharCode; charCode <= group.endCharCode; ++charCode) {
                mapping[charCode] = group.startGlyphCode + (charCode - group.startCharCode);
            }
        }
        return mapping;
    }
    inline static std::map<uint32_t, uint32_t> CreateMapping(FORMAT13* format13) { // Not tested
        std::map<uint32_t, uint32_t> mapping;
        for (uint32_t i = 0; i < format13->nGroups; ++i) {
            FORMAT13::GROUP& group = format13->groups[i];
            for (uint32_t charCode = group.startCharCode; charCode <= group.endCharCode; ++charCode) {
                mapping[charCode] = group.startGlyphCode;
            }
        }
        return mapping;
    }
    inline static std::map<uint32_t, uint32_t> CreateMapping(FORMAT14* format14) { // Not tested
        std::map<uint32_t, uint32_t> mapping;
        for (uint32_t i = 0; i < format14->defaultUVSTable.numUnicodeValueRanges; ++i) {
            FORMAT14::DEFAULTUVSTABLE::UNICODEVALUERANGE& range = format14->defaultUVSTable.unicodeValueRanges[i];
            for (uint32_t j = 0; j <= range.additionalCount; ++j) {
                mapping[range.startUnicodeValue + j] = 0;
            }
        }
        for (uint32_t i = 0; format14->nonDefaultUVSTable.numUVSMappings; ++i) {
            FORMAT14::NONDEFAULTUVSTABLE::UVSMAPPING& fmapping = format14->nonDefaultUVSTable.uvsMappings[i];
            mapping[fmapping.unicodeValue] = fmapping.glyphID;
        }
        return mapping;
    }

    inline static std::vector<vec2> CreateContourWithImpliedPoints(const SIMPLEGLYPH::POINT* points, uint32_t numPoints) {
        uint8_t pointOffset;
        for (pointOffset = 0; pointOffset < numPoints; pointOffset++) {
            if (points[pointOffset].onCurve == true) {
                break;
            }
        }

        std::vector<vec2> new_points;
        for (int i = 0; i < numPoints; i++) {
            SIMPLEGLYPH::POINT currentPoint = points[(i + pointOffset + 0) % numPoints];
            SIMPLEGLYPH::POINT nextPoint = points[(i + pointOffset + 1) % numPoints];
            vec2 new_point = qbn::vectype_cast<double>(currentPoint.coordinates);
            new_points.push_back(new_point);

            if (currentPoint.onCurve == nextPoint.onCurve && i < numPoints) {
                vec2 midpoint = {(currentPoint.coordinates.x + nextPoint.coordinates.x) / 2.0f, (currentPoint.coordinates.y + nextPoint.coordinates.y) / 2.0f};
                new_points.push_back(midpoint);
            }
        }
        return new_points;
    }

    inline static std::vector<TempPoint> CreateContourWithImpliedPointsSimple(const SIMPLEGLYPH::POINT* points, uint32_t numPoints) {
        uint8_t pointOffset;
        for (pointOffset = 0; pointOffset < numPoints; pointOffset++) {
            if (points[pointOffset].onCurve == true) {
                break;
            }
        }

        std::vector<TempPoint> new_points;
        for (int i = 0; i < numPoints; i++) {
            SIMPLEGLYPH::POINT currentPoint = points[(i + pointOffset + 0) % numPoints];
            SIMPLEGLYPH::POINT nextPoint = points[(i + pointOffset + 1) % numPoints];
            vec2 new_point = qbn::vectype_cast<double>(currentPoint.coordinates);
            new_points.push_back({new_point, currentPoint.onCurve});

            if (currentPoint.onCurve == nextPoint.onCurve && i < numPoints) {
                vec2 midpoint = {(currentPoint.coordinates.x + nextPoint.coordinates.x) / 2.0f, (currentPoint.coordinates.y + nextPoint.coordinates.y) / 2.0f};
                new_points.push_back({midpoint, !currentPoint.onCurve});
            }
        }
        return new_points;
    }

    Shape CreateShapeFromSimpleGlyph(SIMPLEGLYPH* simple_glyph, int j, int debug = -1) {
        Shape shape;
        uint16_t startIndex = 0;
        for (int i = 0; i < simple_glyph->numberOfContours; ++i) {
            uint16_t endIndex = simple_glyph->endPtsOfContours[i] + 1;
            Contour contour;
            std::vector<SIMPLEGLYPH::POINT> temp_points(&simple_glyph->points[startIndex], &simple_glyph->points[endIndex]);
            std::vector<TempPoint> points = CreateContourWithImpliedPointsSimple(temp_points.data(), temp_points.size());
            uint16_t numPoints = points.size();

            for (uint16_t i = 0; i < numPoints; ++i) {
                const TempPoint& p0 = points[i];
                const TempPoint& p1 = points[(i + 1) % numPoints];

                // if (j == debug) {
                //     std::cout << "Point: " << p0.coords << " | " << p0.onCurve << '\n';
                //     std::cout << "Point: " << p1.coords << " | " << p1.onCurve << '\n';
                // }

                if (p1.onCurve) {
                    // if (j == debug) {
                    //     std::cout << '2' << std::endl;
                    // }
                    vec2 start = p0.coords;
                    vec2 end = p1.coords;
                    contour.AddEdge(EdgeHolder(start, end));
                    continue;
                }

                const TempPoint& p2 = points[(i + 2) % numPoints];

                // if (j == debug) {
                //     std::cout << "Point: " << p2.coords << " | " << p2.onCurve << '\n';
                // }

                if (p2.onCurve) {
                    // if (j == debug) {
                    //     std::cout << '3' << std::endl;
                    // }
                    vec2 start = p0.coords;
                    vec2 control = p1.coords;
                    vec2 end = p2.coords;
                    contour.AddEdge(EdgeHolder(start, control, end));
                    ++i;
                    continue;
                }

                const TempPoint& p3 = points[(i + 3) % numPoints];

                // if (j == debug) {
                //     std::cout << "Point: " << p3.coords << " | " << p3.onCurve << '\n';              
                // }

                // if (j == debug) {
                //     std::cout << '4' << std::endl;
                // }
                vec2 start = p0.coords;
                vec2 control1 = p1.coords;
                vec2 control2 = p2.coords;
                vec2 end = p3.coords;
                contour.AddEdge(EdgeHolder(start, control1, control2, end));
                i += 2;
            }
            shape.AddContour(contour);
            startIndex = endIndex;
        }
        return shape;
    }
    Shape CreateShapeFromCompoundGlyph(GLYPH** glyphs, uint32_t index) {
        Shape shape;
        COMPOUNDGLYPH* compound_glyph = (COMPOUNDGLYPH*)(glyphs[index]);
        for (int i = 0; i < compound_glyph->numComponentGlyphs; ++i) {
            const COMPONENTGLYPH& component_glyph = compound_glyph->component_glyphs[i];
            const SIMPLEGLYPH* simple_glyph = (SIMPLEGLYPH*)(glyphs[component_glyph.glyphIndex]);
            uint16_t startIndex = 0;
            for (int j = 0; j < simple_glyph->numberOfContours; ++j) {
                uint16_t endIndex = simple_glyph->endPtsOfContours[j] - 1;
                std::vector<SIMPLEGLYPH::POINT> points(&simple_glyph->points[startIndex], &simple_glyph->points[endIndex]);
                std::vector<vec2> d_points = CreateContourWithImpliedPoints(points.data(), points.size()); // points (double type vec)
                uint32_t numPoints = d_points.size();
                Contour contour;
                int type = static_cast<int>(component_glyph.transform->transformType);
                if (type == 0) {
                    for (int k = 0; k < numPoints; k += 2) {
                        vec2 p0 = d_points[k] + qbn::vectype_cast<double>(component_glyph.offset);
                        vec2 p1 = d_points[(k + 1) % numPoints] + qbn::vectype_cast<double>(component_glyph.offset);
                        vec2 p2 = d_points[(k + 2) % numPoints] + qbn::vectype_cast<double>(component_glyph.offset);
                        contour.AddEdge(EdgeHolder(p0, p1, p2));
                    }
                } else if (type == 1) {
                    TRANSFORM1& transform = *static_cast<TRANSFORM1*>(component_glyph.transform);
                    for (int k = 0; k < numPoints; k += 2) {
                        vec2 p0 = transform.scale * d_points[k] + qbn::vectype_cast<double>(component_glyph.offset);
                        vec2 p1 = transform.scale * d_points[(k + 1) % numPoints] + qbn::vectype_cast<double>(component_glyph.offset);
                        vec2 p2 = transform.scale * d_points[(k + 2) % numPoints] + qbn::vectype_cast<double>(component_glyph.offset);
                        contour.AddEdge(EdgeHolder(p0, p1, p2));
                    }
                } else if (type == 2) {
                    TRANSFORM2& transform = *static_cast<TRANSFORM2*>(component_glyph.transform);
                    vec2 scale = transform.scale;
                    for (int k = 0; k < numPoints; k += 2) {
                        vec2 p0 = scale * d_points[k] + qbn::vectype_cast<double>(component_glyph.offset);
                        vec2 p1 = scale * d_points[(k + 1) % numPoints] + qbn::vectype_cast<double>(component_glyph.offset);
                        vec2 p2 = scale * d_points[(k + 2) % numPoints] + qbn::vectype_cast<double>(component_glyph.offset);
                        contour.AddEdge(EdgeHolder(p0, p1, p2));
                    }
                } else if (type == 3) {
                    TRANSFORM3& transform = *static_cast<TRANSFORM3*>(component_glyph.transform);
                    qbn::mat<double, 2> scale = transform.scale;
                    for (int k = 0; k < numPoints; k += 2) {
                        vec2 p0 = scale * d_points[k] + qbn::vectype_cast<double>(component_glyph.offset);
                        vec2 p1 = scale * d_points[(k + 1) % numPoints] + qbn::vectype_cast<double>(component_glyph.offset);
                        vec2 p2 = scale * d_points[(k + 2) % numPoints] + qbn::vectype_cast<double>(component_glyph.offset);
                        contour.AddEdge(EdgeHolder(p0, p1, p2));
                    }
                } else {
                    throw std::runtime_error("Invalid transformation type.");
                }
                
                shape.AddContour(contour);
                startIndex = endIndex + 1;
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

    MSDFFont::MSDFFont(const std::string& path) {
        Reader<TTF> reader;
        TrueTypeFont ttf = reader.CreateTTFFromFile(path);
        metrics.emSize = double(ttf.head.unitsPerEm);
        metrics.ascenderY = double(ttf.hhea.ascent);
        metrics.descenderY = double(ttf.hhea.descent);
        metrics.lineHeight = double(ttf.hhea.ascent - ttf.hhea.descent + ttf.hhea.lineGap);
        metrics.underlineY = double(ttf.post.underlinePosition);
        metrics.underlineThickness = double(ttf.post.underlineThickness);

        for (int i = 0; i < ttf.glyf.numGlyphs; ++i) {
            Shape shape = ttf.glyf.glyphs[i]->glyphType ? CreateShapeFromCompoundGlyph(ttf.glyf.glyphs, i) : CreateShapeFromSimpleGlyph(dynamic_cast<SIMPLEGLYPH*>(ttf.glyf.glyphs[i]), i, 816);
            // if (i == 816) {
            //     for (auto& contour : shape.contours) {
            //         std::cout << "Contour: " << std::endl;
            //         for (auto& edge : contour.edges) {
            //             auto points = edge->ControlPoints();
            //             for (int i = 0; i < (*edge).Type(); ++i) {
            //                 auto point = points[i];
            //                 std::cout << "(" << point.x << ", " << point.y << ")" << std::endl;
            //             }
            //         }
            //     }
            // }
            GlyphMetrics glyphMetrics = GetMetrics(ttf, i);
            // ResolveShapeGeometry(shape);
            glyphs.push_back(Glyph(shape, glyphMetrics, i));
        }
        if (ttf.cmap.numberSubtables > 0) {
            SUBTABLE& subtable = ttf.cmap.subtables[0];
            switch (subtable.formatID) {
                case 0: mapping = CreateMapping(static_cast<FORMAT0*>(subtable.format)); break;
                case 2: mapping = CreateMapping(static_cast<FORMAT2*>(subtable.format)); break;
                case 4: mapping = CreateMapping(static_cast<FORMAT4*>(subtable.format)); break;
                case 6: mapping = CreateMapping(static_cast<FORMAT6*>(subtable.format)); break;
                case 8: mapping = CreateMapping(static_cast<FORMAT8*>(subtable.format)); break;
                case 10: mapping = CreateMapping(static_cast<FORMAT10*>(subtable.format)); break;
                case 12: mapping = CreateMapping(static_cast<FORMAT12*>(subtable.format)); break;
                case 13: mapping = CreateMapping(static_cast<FORMAT13*>(subtable.format)); break;
                case 14: mapping = CreateMapping(static_cast<FORMAT14*>(subtable.format)); break;
            }
        } else {
            throw std::runtime_error("Invalid number of subtables.");
        }
        for (int i = 0; i < ttf.name.count; ++i) {
            NAME::NAMERECORD& nameRecord = ttf.name.nameRecords[i];
            if (nameRecord.nameID == 4) {
                name = nameRecord.name;
                break;
            }
        }
        if (ttf.kern.offset != 0) {
            kerning = CreateKerning(ttf);
        }

        Glyph& whitespace = GetGlyph(32);
        auto index = whitespace.index;
        auto advanceWidth = whitespace.metrics.advanceWidth;
        GlyphMetrics whitespaceMetrics{};
        whitespaceMetrics.advanceWidth = advanceWidth;
        
        whitespace = Glyph(whitespaceShape, whitespaceMetrics, index);
    }

    MSDFFont::~MSDFFont() {
        glyphs.clear();
    }

    Glyph& MSDFFont::GetGlyph(uint32_t unicode) {
        auto it = mapping.find(unicode);
        if (it == mapping.end()) {
            throw std::runtime_error("Could not find glyph in map.");
        }
        return glyphs[it->second];
    }

    std::vector<Glyph>& MSDFFont::GetGlyphs() {
        return glyphs;
    }

    const FontMetrics& MSDFFont::GetFontMetrics() const {
        return metrics;
    }
    
    double MSDFFont::GetKerning(uint32_t codepointA, uint32_t codepointB, double geometryScale) {
        return double(kerning[{codepointA, codepointB}]) * geometryScale;
    }

    double MSDFFont::GetAdvance(uint32_t codepointA, uint32_t codepointB, double geometryScale) {
        return GetGlyph(codepointA).GetAdvance() * geometryScale + GetKerning(codepointA, codepointB, geometryScale);
    }

    const std::map<std::pair<int, int>, int>& MSDFFont::GetKerningMap() const {
        return kerning;
    }
    
}