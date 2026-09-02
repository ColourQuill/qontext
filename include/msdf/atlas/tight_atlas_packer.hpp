#ifndef __QONTEXT_TIGHT_ATLAS_PACKER_HPP__
#define __QONTEXT_TIGHT_ATLAS_PACKER_HPP__

// msdf
#include <msdf/atlas/types.hpp>
#include <msdf/atlas/padding.hpp>
#include <msdf/atlas/glyph_geometry.hpp>

namespace qtxt {
    class TightAtlasPacker {
        public:
            TightAtlasPacker();

            int pack(std::vector<GlyphGeometry*>& glyphs);

            void setDimensions(int width, int height);
            void unsetDimensions();
            void setDimensionsConstraint(DimensionsConstraint dimensionsConstraint);

            void setSpacing(int spacing);

            void setScale(double scale);
            void setMinimumScale(double minScale);

            void setUnitRange(Range unitRange);
            void setPixelRange(Range pxRange);

            void setMiterLimit(double miterLimit);

            void setOriginPixelAlignment(bool align);
            void setOriginPixelAlignment(bool alignX, bool alignY);

            void setInnerUnitPadding(const Padding& padding);
            void setOuterUnitPadding(const Padding& padding);
            void setInnerPixelPadding(const Padding& padding);
            void setOuterPixelPadding(const Padding& padding);

            void getDimensions(int& width, int& height) const;
            double getScale() const;
            Range getPixelRange() const;
        private:
            int width, height;
            int spacing;
            DimensionsConstraint dimensionsConstraint;
            double scale;
            double minScale;
            Range unitRange;
            Range pxRange;
            double miterLimit;
            bool pxAlignOriginX, pxAlignOriginY;
            Padding innerUnitPadding, outerUnitPadding;
            Padding innerPxPadding, outerPxPadding;
            double scaleMaximizationTolerance;

            int tryPack(std::vector<GlyphGeometry*>& glyphs, DimensionsConstraint dimensionsConstraint, int& width, int& height, double scale) const;
            double packAndScale(std::vector<GlyphGeometry*>& glyphs) const;
    };
}

#endif