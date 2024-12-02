#ifndef __QONTEXT_TIGHT_ATLAS_PACKER_HPP__
#define __QONTEXT_TIGHT_ATLAS_PACKER_HPP__

// msdf
#include <generator/msdf/atlas/types.hpp>
#include <generator/msdf/atlas/padding.hpp>
#include <generator/msdf/atlas/glyph_geometry.hpp>

namespace qtxt {
    class TightAtlasPacker {
        public:
            TightAtlasPacker();

            int Pack(std::vector<GlyphGeometry*>& glyphs);

            void SetDimensions(int width, int height);
            void UnsetDimensions();
            void SetDimensionsConstraint(DimensionsConstraint dimensionsConstraint);

            void SetSpacing(int spacing);

            void SetScale(double scale);
            void SetMinimumScale(double minScale);

            void SetUnitRange(Range unitRange);
            void SetPixelRange(Range pxRange);

            void SetMiterLimit(double miterLimit);

            void SetOriginPixelAlignment(bool align);
            void SetOriginPixelAlignment(bool alignX, bool alignY);

            void SetInnerUnitPadding(const Padding& padding);
            void SetOuterUnitPadding(const Padding& padding);
            void SetInnerPixelPadding(const Padding& padding);
            void SetOuterPixelPadding(const Padding& padding);

            void GetDimensions(int& width, int& height) const;
            double GetScale() const;
            Range GetPixelRange() const;
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

            int TryPack(std::vector<GlyphGeometry*>& glyphs, DimensionsConstraint dimensionsConstraint, int& width, int& height, double scale) const;
            double PackAndScale(std::vector<GlyphGeometry*>& glyphs) const;
    };
}

#endif