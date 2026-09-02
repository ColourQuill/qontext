#ifndef __QONTEXT_TTF_KERN_HPP__
#define __QONTEXT_TTF_KERN_HPP__

// qontext
#include <info/type_defs.hpp>

namespace qtxt {
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

        struct StateHeader {
            uint16_t stateSize; // 'Size of a state, in bytes. The size is limited to 8 bits, although the field is 16 bits for alignment.'
            uint16_t classTable; // 'Byte offset from the beginning of the state table to the class subtable.'
            uint16_t stateArray; // 'Byte offset from the beginning of the state table to the state array.'
            uint16_t entryTable; // 'Byte offset from the beginning of the state table to the entry subtable.'
        };

        struct Subtable {
            virtual ~Subtable() = default;
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

        struct Subtable0 : public Subtable {
            ~Subtable0() {
                if (pairs != nullptr) {
                    delete[] pairs;
                }
            }

            uint16_t nPairs; // 'The number of kerning pairs in this subtable.'
            uint16_t searchRange; // 'The largest power of two less than or equal to the value of nPairs, multiplied by the size in bytes of an entry in the subtable.'
            uint16_t entrySelector; // 'This is calculated as long2 of the largest power of two less than or equal to the value of nPairs. This value indicates how many iterations of the search loop have to be made. For example, in a list of eight items, there would be three iterations of the loop.'
            uint16_t rangeShift; // 'The value of nPairs minus the largest power of two less than or equal to nPairs. This is multiplied by the size in bytes of an entry in the table.'

            struct Pair {
                uint16_t left; // 'The glyph index for the lefthand glyph in the kerning pair.'
                uint16_t right; // 'The glyph index for the righthand glyph in the kerning pair.'
                int16_t value; // 'The kerning value in FUnits for the left and right pair in FUnits. If this value is greater than zero, the glyphs are moved apart. If this value is less than zero, the glyphs are moved together.'
            };

            Pair* pairs{nullptr};
        };
        
        struct Subtable1 : public Subtable { // Currently unsupported
            StateHeader stHeader; // 'The contextual kerning state table header.'
            uint16_t valueTable; // 'Offset in bytes from the beginning of the subtable to the beginning of the kerning table.'
        };

        struct Subtable2 : public Subtable { // Currently unsupported
            
        };

        struct Subtable3 : public Subtable { // Currently unsupported

        };

        Subtable** subtables{nullptr};
    };
}

#endif