#ifndef __TTF_HEAD_HPP__
#define __TTF_HEAD_HPP__

// qarbon
#include <qarbon.hpp>

// qontext
#include <info/type_defs.hpp>

namespace qtxt {
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
}

#endif