#include <generator/msdf/atlas/tight_atlas_packer.hpp>

// std
#include <vector>
#include <iostream>

// msdf
#include <generator/msdf/atlas/rectangle.hpp>
#include <generator/msdf/atlas/rectangle_packing.hpp>
#include <generator/msdf/atlas/size_selectors.hpp>

namespace qtxt {
    TightAtlasPacker::TightAtlasPacker() : 
        width{-1}, height{-1},
        spacing{0},
        dimensionsConstraint{DimensionsConstraint::POWER_OF_TWO_SQUARE},
        scale{-1},
        minScale{1},
        unitRange{0},
        pxRange{0},
        miterLimit{0},
        pxAlignOriginX{false}, pxAlignOriginY{false},
        scaleMaximizationTolerance{0.001}
    {}

    int TightAtlasPacker::TryPack(std::vector<GlyphGeometry*>& glyphs, DimensionsConstraint dimensionsConstraint, int& width, int& height, double scale) const {
        std::vector<Rectangle> rectangles;
        std::vector<GlyphGeometry*> rectangleGlyphs;
        rectangles.reserve(glyphs.size());
        rectangleGlyphs.reserve(glyphs.size());
        GlyphGeometry::GlyphAttributes attribs = {};
        attribs.scale = scale;
        attribs.range = unitRange + pxRange / scale;
        attribs.innerPadding = innerUnitPadding + innerPxPadding / scale;
        attribs.outerPadding = outerUnitPadding + outerPxPadding / scale;
        attribs.miterLimit = miterLimit;
        attribs.pxAlignOriginX = pxAlignOriginX;
        attribs.pxAlignOriginY = pxAlignOriginY;
        for (auto& glyph : glyphs) {
            if (!glyph->isWhitespace()) {
                Rectangle rect = {};
                glyph->WrapBox(attribs);      
                glyph->GetBoxSize(rect.w, rect.h);
                if (rect.w > 0 && rect.h > 0) {
                    rectangles.push_back(rect);
                    rectangleGlyphs.push_back(glyph);
                }
            }
        }

        if (rectangles.empty()) {
            if (width < 0 || height < 0) {
                width = 0, height = 0;
            }
            return 0;
        }

        if (width < 0 || height < 0) {
            std::pair<int, int> dimensions = std::make_pair(width, height);
            switch (dimensionsConstraint) {
                case DimensionsConstraint::POWER_OF_TWO_SQUARE:
                    dimensions = PackRectangles<SquarePowerOfTwoSizeSelector>(rectangles.data(), rectangles.size(), spacing);
                    break;
                case DimensionsConstraint::POWER_OF_TWO_RECTANGLE:
                    dimensions = PackRectangles<PowerOfTwoSizeSelector>(rectangles.data(), rectangles.size(), spacing);
                    break;
                case DimensionsConstraint::MULTIPLE_OF_FOUR_SQUARE:
                    dimensions = PackRectangles<SquareSizeSelector<4>>(rectangles.data(), rectangles.size(), spacing);
                    break;
                case DimensionsConstraint::EVEN_SQUARE:
                    dimensions = PackRectangles<SquareSizeSelector<2>>(rectangles.data(), rectangles.size(), spacing);
                    break;
                case DimensionsConstraint::SQUARE:
                default:
                    dimensions = PackRectangles<SquareSizeSelector<>>(rectangles.data(), rectangles.size(), spacing);
                    break;
            }
            if (!(dimensions.first > 0 && dimensions.second > 0)) {
                return -1;
            }
            width = dimensions.first, height = dimensions.second;
        } else {
            if (int result = PackRectangles(rectangles.data(), rectangles.size(), width, height, spacing)) {
                return result;
            }
        }

        for (size_t i = 0; i < rectangles.size(); ++i) {
            rectangleGlyphs[i]->PlaceBox(rectangles[i].x, height - (rectangles[i].y + rectangles[i].h));
        }

        return 0;
    }

    double TightAtlasPacker::PackAndScale(std::vector<GlyphGeometry*>& glyphs) const {
        bool lastResult = false;
        int w = width, h = height;
        #define TRY_PACK(scale) (lastResult = !TryPack(glyphs, DimensionsConstraint(), w, h, (scale)))
        double minScale = 1, maxScale = 1;
        if (TRY_PACK(1)) {
            while (maxScale < 1e+32 && ((maxScale = 2 * minScale), TRY_PACK(maxScale))) {
                minScale = maxScale;
            }
        } else {
            while (minScale > 1e-32 && ((minScale = 0.5 * maxScale), !TRY_PACK(minScale))) {
                maxScale = minScale;
            }
        }
        if (minScale == maxScale) {
            return 0;
        }
        while (minScale / maxScale < 1 - scaleMaximizationTolerance) {
            double midScale = 0.5 * (minScale + maxScale);
            if (TRY_PACK(midScale)) {
                minScale = midScale;
            } else {
                maxScale = midScale;
            }
        }
        if (!lastResult) {
            TRY_PACK(minScale);
        }
        return minScale;
    }

    int TightAtlasPacker::Pack(std::vector<GlyphGeometry*>& glyphs) {
        double initialScale = scale > 0 ? scale : minScale;
        if (initialScale > 0) {
            if (int remaining = TryPack(glyphs, dimensionsConstraint, width, height, initialScale)) {
                return remaining;
            }
        } else if (width < 0 || height < 0) {
            return -1;
        }
        if (scale <= 0) {
            scale = PackAndScale(glyphs);
        }
        if (scale <= 0) {
            return -1;
        }
        return 0;
    }

    void TightAtlasPacker::SetDimensions(int width, int height) {
        this->width = width, this->height = height;
    }
    void TightAtlasPacker::UnsetDimensions() {
        width = -1, height = -1;
    }
    void TightAtlasPacker::SetDimensionsConstraint(DimensionsConstraint dimensionsConstraint) {
        this->dimensionsConstraint = dimensionsConstraint;
    }

    void TightAtlasPacker::SetSpacing(int spacing) {
        this->spacing = spacing;
    }

    void TightAtlasPacker::SetScale(double scale) {
        this->scale = scale;
    }
    void TightAtlasPacker::SetMinimumScale(double minScale) {
        this->minScale = minScale;
    }

    void TightAtlasPacker::SetUnitRange(Range unitRange) {
        this->unitRange = unitRange;
    }
    void TightAtlasPacker::SetPixelRange(Range pxRange) {
        this->pxRange = pxRange;
    }

    void TightAtlasPacker::SetMiterLimit(double miterLimit) {
        this->miterLimit = miterLimit;
    }

    void TightAtlasPacker::SetOriginPixelAlignment(bool align) {
        pxAlignOriginX = align, pxAlignOriginY = align;
    }
    void TightAtlasPacker::SetOriginPixelAlignment(bool alignX, bool alignY) {
        pxAlignOriginX = alignX, pxAlignOriginY = alignY;
    }

    void TightAtlasPacker::SetInnerUnitPadding(const Padding& padding) {
        innerUnitPadding = padding;
    }
    void TightAtlasPacker::SetOuterUnitPadding(const Padding& padding) {
        outerUnitPadding = padding;
    }
    void TightAtlasPacker::SetInnerPixelPadding(const Padding& padding) {
        innerPxPadding = padding;
    }
    void TightAtlasPacker::SetOuterPixelPadding(const Padding& padding) {
        outerPxPadding = padding;
    }

    void TightAtlasPacker::GetDimensions(int& width, int& height) const {
        width = this->width, height = this->height;
    }
    double TightAtlasPacker::GetScale() const {
        return scale;
    }
    Range TightAtlasPacker::GetPixelRange() const {
        return pxRange + scale * unitRange;
    }
}