#ifndef __QONTEXT_TTF_GLYPH_HPP__
#define __QONTEXT_TTF_GLYPH_HPP__

// std
#include <info/type_defs.hpp>

// qarbon
#include <qarbon.hpp>

namespace qtxt {
    struct TTFGlyph {
        virtual ~TTFGlyph() = default;
        uint8_t glyphType;
    };

    struct Point {
        qbn::vec<int16_t, 2> coordinates;
        bool onCurve;
    };

    struct SimpleGlyph : public TTFGlyph {
        ~SimpleGlyph() {
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

        Point* points{nullptr};
    };

    struct ComponentGlyph {
        struct Transform {
            virtual ~Transform() = default;
            uint8_t transformType;
        };
        struct Transform0 : public Transform {
            double scale = 1;
        };
        struct Transform1 : public Transform {
            double scale;
        };
        struct Transform2 : public Transform {
            qbn::vec2<double> scale;
        };
        struct Transform3 : public Transform {
            qbn::mat<double, 2> scale;
        };

        ~ComponentGlyph() {
            if (transform != nullptr) {
                delete transform;
            }
        }

        uint16_t flags;
        uint16_t glyphIndex;
        qbn::vec2<int16_t> offset;
        Transform* transform{nullptr};
    };

    struct CompoundGlyph : public TTFGlyph {
        int16_t numberOfContours; // 'If the number of contours is positive or zero, it is a simple glyph; If the number of contours less than zero, the glyph is compound'
        qbn::vec2<FWord> min; // Minimum coordinate data
        qbn::vec2<FWord> max; // Maximum coordinate data
        ComponentGlyph* component_glyphs{nullptr};
        uint32_t numComponentGlyphs;
    };
}

#endif