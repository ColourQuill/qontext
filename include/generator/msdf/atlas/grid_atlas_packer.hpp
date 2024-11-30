#ifndef __QONTEXT_GRID_ATLAS_PACKER_HPP__
#define __QONTEXT_GRID_ATLAS_PACKER_HPP__

// msdf
#include <generator/msdf/atlas/padding.hpp>
#include <generator/msdf/atlas/glyph_geometry.hpp>

namespace qtxt {
    class GridAtlasPacker {
        public:
            int Pack(GlyphGeometry* glyphs, int count);

            void SetFixedOrigin(bool horizontal, bool vertical);
            void SetCellDimensions(int width, int height);
            void UnsetCellDimensions();
            void SetCellDimensionsConstraint(DimensionsConstraint dimensionsConstraint);
            void SetColumns(int columns);
            void SetRows(int rows);
            void UnsetColumns();
            void UnsetRows();

            void SetDimensions(int width, int height);
            void UnsetDimensions();
            void SetDimensionsConstraint(DimensionsConstraint dimensionsConstraint);

            void SetSpacing(int spacing);

            void SetScale(double scale);
            void SetMinimumScale(double scale);

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
            void GetCellDimensions(int& width, int& height) const;

            int GetColumns() const;
            int GetRows() const;

            double GetScale() const;

            Range GetPixelRange() const;

            void GetFixedOrigin(double& x, double& y);

            bool HasCutoff() const;

        private:
            int columns, rows;
            int width, height;
            int cellWidth, cellHeight;
            int spacing;
            DimensionsConstraint dimensionsConstraint;
            DimensionsConstraint cellDimensionsConstraint;
            bool hFixed, vFixed;
            double scale;
            double minScale;
            double fixedX, fixedY;
            Range unitRange;
            Range pxRange;
            double miterLimit;
            bool pxAlignOriginX, pxAlignOriginY;
            Padding innerUnitPadding, outerUnitPadding;
            Padding innerPxPadding, outerPxPadding;
            double scaleMaximizationTolerance;
            double alignedColumnsBias;
            bool cutoff;

            static void LowerToConstraint(int& width, int& height, DimensionsConstraint constraint);
            static void RaiseToConstraint(int& width, int& height, DimensionsConstraint constraint);

            double DimensionsRating(int width, int height, bool aligned) const;
            Bounds GetMaxBounds(double& maxWidth, double& maxHeight, GlyphGeometry* glyphs, int count, double scale, double outerRange) const;
            double ScaleToFit(GlyphGeometry* glyphs, int count, int cellWidth, int cellHeight, Bounds& maxBounds, double& maxWidth, double& maxHeight) const;
    };
}

#endif