#ifndef __QONTEXT_GLYPH_GEOMETRY_HPP__
#define __QONTEXT_GLYPH_GEOMETRY_HPP__

// msdf
#include <msdf/core/glyph.hpp>
#include <msdf/core/projection.hpp>
#include <msdf/core/shape/shape.hpp>
#include <msdf/atlas/glyph_attributes.hpp>
#include <msdf/atlas/glyph_box.hpp>
#include <msdf/atlas/types.hpp>

namespace qtxt {
    class GlyphGeometry {
        public:
            GlyphGeometry(Glyph& glyph, uint32_t codepoint, double geometryScale);

            void edgeColoring(void (*fn)(Shape&, double, unsigned long long), double angleThreshold, unsigned long long seed);

            void wrapBox(const GlyphAttributes& glyphAttributes);
            void wrapBox(double scale, double range, double miterLimit, bool pxAlignOrigin = false);
            void wrapBox(double scale, double range, double miterLimit, bool pxAlignOriginX, bool pxAlignOriginY);

            void frameBox(const GlyphAttributes& glyphAttributes, int width, int height, const double* fixedX, const double* fixedY);
            void frameBox(double scale, double range, double miterLimit, int width, int height, const double* fixedX, const double* fixedY, bool pxAlignOrigin = false);
            void frameBox(double scale, double range, double miterLimit, int width, int height, const double* fixedX, const double* fixedY, bool pxAlignOriginX, bool pxAlignOriginY);

            void placeBox(int x, int y);
            void setBoxRect(const Rectangle& rect);

            int getIndex() const;
            uint32_t getCodepoint() const;
            int getIdentifier(GlyphIdentifierType type) const;

            double getGeometryScale() const;
            Shape& getShape() const;
            const Bounds& getShapeBounds() const;

            double getAdvance() const;
            Rectangle getBoxRect() const;
            void getBoxRect(int& x, int& y, int& w, int& h) const;
            void getBoxSize(int& w, int& h) const;
            Range getBoxRange() const;
            Projection getBoxProjection() const;
            double getBoxScale() const;
            vec2 getBoxTranslate() const;

            void getQuadPlaneBounds(double& l, double& b, double& r, double& t) const;
            void getQuadAtlasBounds(double& l, double& b, double& r, double& t) const;

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
}

#endif