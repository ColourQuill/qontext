#ifndef __QONTEXT_GLYPH_GEOMETRY_HPP__
#define __QONTEXT_GLYPH_GEOMETRY_HPP__

// msdf
#include <generator/msdf/core/bounds.hpp>
#include <generator/msdf/core/range.hpp>
#include <generator/msdf/core/projection.hpp>
#include <generator/msdf/core/msdf_font.hpp>
#include <generator/msdf/atlas/types.hpp>
#include <generator/msdf/atlas/padding.hpp>
#include <generator/msdf/atlas/rectangle.hpp>
#include <generator/msdf/atlas/glyph_box.hpp>

namespace qtxt {
    class GlyphGeometry {
        public:
            struct GlyphAttributes {
                double scale;
                Range range;
                Padding innerPadding, outerPadding;
                double miterLimit;
                bool pxAlignOriginX, pxAlignOriginY;
            };

            GlyphGeometry(Glyph& glyph, uint32_t codepoint, double geometryScale);

            void EdgeColoring(void (*fn)(Shape&, double, unsigned long long), double angleThreshold, unsigned long long seed);

            void WrapBox(const GlyphAttributes& glyphAttributes);
            void WrapBox(double scale, double range, double miterLimit, bool pxAlignOrigin = false);
            void WrapBox(double scale, double range, double miterLimit, bool pxAlignOriginX, bool pxAlignOriginY);

            void FrameBox(const GlyphAttributes& glyphAttributes, int width, int height, const double* fixedX, const double* fixedY);
            void FrameBox(double scale, double range, double miterLimit, int width, int height, const double* fixedX, const double* fixedY, bool pxAlignOrigin = false);
            void FrameBox(double scale, double range, double miterLimit, int width, int height, const double* fixedX, const double* fixedY, bool pxAlignOriginX, bool pxAlignOriginY);

            void PlaceBox(int x, int y);

            void SetBoxRect(const Rectangle& rect);

            int GetIndex() const;

            uint32_t GetCodepoint() const;

            int GetIdentifier(GlyphIdentifierType type) const;

            double GetGeometryScale() const;

            Shape& GetShape() const;

            const Bounds& GetShapeBounds() const;

            double GetAdvance() const;

            Rectangle GetBoxRect() const;

            void GetBoxRect(int& x, int& y, int& w, int& h) const;
            
            void GetBoxSize(int& w, int& h) const;

            Range GetBoxRange() const;

            Projection GetBoxProjection() const;

            double GetBoxScale() const;

            vec2 GetBoxTranslate() const;

            void GetQuadPlaneBounds(double& l, double& b, double& r, double& t) const;

            void GetQuadAtlasBounds(double& l, double& b, double& r, double& t) const;

            bool isWhitespace() const;

            operator GlyphBox() const;

        private:
            int index;
            uint32_t codepoint;
            double geometryScale;
            Shape& shape;
            Bounds bounds;
            double advance;
            struct {
                Rectangle rect;
                Range range;
                double scale;
                vec2 translate;
                Padding outerPadding;
            } box;
    };

    Range operator+(Range a, Range b);
}

#endif