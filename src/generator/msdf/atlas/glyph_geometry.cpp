#include <generator/msdf/atlas/glyph_geometry.hpp>

// msdf
#include <generator/msdf/core/glyph.hpp>

namespace qtxt {
    GlyphGeometry::GlyphGeometry(Glyph& glyph, uint32_t codepoint, double geometryScale) : shape{glyph.shape}, geometryScale{geometryScale}, codepoint{codepoint} {
        advance = glyph.GetAdvance();
        index = glyph.index;
        advance *= geometryScale;
        shape.Normalize();
        bounds = glyph.GetBounds();
    }

    void GlyphGeometry::EdgeColoring(void (*fn)(Shape&, double, unsigned long long), double angleThreshold, unsigned long long seed) {
        fn(shape, angleThreshold, seed);
    }

    void GlyphGeometry::WrapBox(const GlyphAttributes& glyphAttributes) {
        double scale = glyphAttributes.scale * geometryScale;
        Range range = glyphAttributes.range / geometryScale;
        Padding fullPadding = (glyphAttributes.innerPadding + glyphAttributes.outerPadding) / geometryScale;
        box.range = range;
        box.scale = scale;
        if (bounds.l < bounds.r && bounds.b < bounds.t) {
            double l = bounds.l, b = bounds.b, r = bounds.r, t = bounds.t;
            l += range.lower, b += range.lower;
            r -= range.lower, t -= range.lower;
            if (glyphAttributes.miterLimit > 0) {
                shape.BoundMiters(l, b, r, t, -range.lower, glyphAttributes.miterLimit, 1);
            }
            l -= fullPadding.l, b -= fullPadding.b;
            r += fullPadding.r, t += fullPadding.t;
            if (glyphAttributes.pxAlignOriginX) {
                int sl = (int)floor(scale * l - 0.5);
                int sr = (int)ceil(scale * r + 0.5);
                box.rect.w = sr - sl;
                box.translate.x = -sl / scale;
            } else {
                double w = scale * (r - l);
                box.rect.w = (int)ceil(w) + 1;
                box.translate.x = -l + 0.5 * (box.rect.w - w) / scale;
            }
            if (glyphAttributes.pxAlignOriginY) {
                int sb = (int)floor(scale * b - 0.5);
                int st = (int)ceil(scale * t + 0.5);
                box.rect.h = st - sb;
                box.translate.y = -sb / scale;
            } else {
                double h = scale * (t - b);
                box.rect.h = (int)ceil(h) + 1;
                box.translate.y = -b + 0.5 * (box.rect.h - h) / scale;
            }
            box.outerPadding = glyphAttributes.scale * glyphAttributes.outerPadding;
        } else {
            box.rect.w = 0, box.rect.h = 0;
            box.translate = vec2();
        }
    }
    void GlyphGeometry::WrapBox(double scale, double range, double miterLimit, bool pxAlignOrigin) {
        GlyphAttributes attribs = {};
        attribs.scale = scale;
        attribs.range = range;
        attribs.miterLimit = miterLimit;
        attribs.pxAlignOriginX = pxAlignOrigin;
        attribs.pxAlignOriginY = pxAlignOrigin;
        WrapBox(attribs);
    }
    void GlyphGeometry::WrapBox(double scale, double range, double miterLimit, bool pxAlignOriginX, bool pxAlignOriginY) {
        GlyphAttributes attribs = {};
        attribs.scale = scale;
        attribs.range = range;
        attribs.miterLimit = miterLimit;
        attribs.pxAlignOriginX = pxAlignOriginX;
        attribs.pxAlignOriginY = pxAlignOriginY;
        WrapBox(attribs);
    }

    void GlyphGeometry::FrameBox(const GlyphAttributes& glyphAttributes, int width, int height, const double* fixedX, const double* fixedY) {
        double scale = glyphAttributes.scale * geometryScale;
        Range range = glyphAttributes.range / geometryScale;
        Padding fullPadding = (glyphAttributes.innerPadding + glyphAttributes.outerPadding) / geometryScale;
        box.range = range;
        box.scale = scale;
        box.rect.w = width;
        box.rect.h = height;
        if (fixedX && fixedY) {
            box.translate.x = *fixedX / geometryScale;
            box.translate.y = *fixedY / geometryScale;
        } else {
            double l = bounds.l, b = bounds.b, r = bounds.r, t = bounds.t;
            l += range.lower, b += range.lower;
            r -= range.lower, t -= range.lower;
            if (glyphAttributes.miterLimit > 0) {
                shape.BoundMiters(l, b, r, t, -range.lower, glyphAttributes.miterLimit, 1);
            }
            l -= fullPadding.l, b -= fullPadding.b;
            r += fullPadding.r, t += fullPadding.t;
            if (fixedX) {
                box.translate.x = *fixedX / geometryScale;
            }
            else if (glyphAttributes.pxAlignOriginX) {
                int sl = (int)floor(scale * l - 0.5);
                int sr = (int)ceil(scale * r + 0.5);
                box.translate.x = (-sl + (box.rect.w - (sr - sl)) / 2) / scale;
            } else {
                double w = scale * (r - l);
                box.translate.x = - l + 0.5 * (box.rect.w - w) / scale;
            }
            if (fixedY) {
                box.translate.y = *fixedY / geometryScale;
            }
            else if (glyphAttributes.pxAlignOriginY) {
                int sb = (int)floor(scale * b - 0.5);
                int st = (int)ceil(scale * t + 0.5);
                box.translate.y = (-sb + (box.rect.h - (st - sb)) / 2) / scale;
            } else {
                double h = scale * (t - b);
                box.translate.y = -b + 0.5 * (box.rect.h - h) / scale;
            }
        }
        box.outerPadding = glyphAttributes.scale * glyphAttributes.outerPadding;
    }
    void GlyphGeometry::FrameBox(double scale, double range, double miterLimit, int width, int height, const double* fixedX, const double* fixedY, bool pxAlignOrigin) {
        GlyphAttributes attribs = {};
        attribs.scale = scale;
        attribs.range = range;
        attribs.miterLimit = miterLimit;
        attribs.pxAlignOriginX = pxAlignOrigin;
        attribs.pxAlignOriginY = pxAlignOrigin;
        FrameBox(attribs, width, height, fixedX, fixedY);
    }
    void GlyphGeometry::FrameBox(double scale, double range, double miterLimit, int width, int height, const double* fixedX, const double* fixedY, bool pxAlignOriginX, bool pxAlignOriginY) {
        GlyphAttributes attribs = {};
        attribs.scale = scale;
        attribs.range = range;
        attribs.miterLimit = miterLimit;
        attribs.pxAlignOriginX = pxAlignOriginX;
        attribs.pxAlignOriginY = pxAlignOriginY;
        FrameBox(attribs, width, height, fixedX, fixedY);
    }

    void GlyphGeometry::PlaceBox(int x, int y) {
        box.rect.x = x, box.rect.y = y;
    }

    void GlyphGeometry::SetBoxRect(const Rectangle& rect) {
        box.rect = rect;
    }

    int GlyphGeometry::GetIndex() const {
        return index;
    }

    uint32_t GlyphGeometry::GetCodepoint() const {
        return codepoint;
    }

    int GlyphGeometry::GetIdentifier(GlyphIdentifierType type) const {
        switch (type) {
            case GlyphIdentifierType::GLYPH_INDEX:
                return index;
            case GlyphIdentifierType::UNICODE_CODEPOINT:
                return codepoint;
        }
        return 0;
    }

    double GlyphGeometry::GetGeometryScale() const {
        return geometryScale;
    }

    Shape& GlyphGeometry::GetShape() const {
        return shape;
    }

    const Bounds& GlyphGeometry::GetShapeBounds() const {
        return bounds;
    }

    double GlyphGeometry::GetAdvance() const {
        return advance;
    }

    Rectangle GlyphGeometry::GetBoxRect() const {
        return box.rect;
    }

    void GlyphGeometry::GetBoxRect(int& x, int& y, int& w, int& h) const {
        x = box.rect.x, y = box.rect.y;
        w = box.rect.w, h = box.rect.h;
    }
    
    void GlyphGeometry::GetBoxSize(int& w, int& h) const {
        w = box.rect.w, h = box.rect.h;
    }

    Range GlyphGeometry::GetBoxRange() const {
        return box.range;
    }

    Projection GlyphGeometry::GetBoxProjection() const {
        return Projection(vec2(box.scale, box.scale), box.translate);
    }

    double GlyphGeometry::GetBoxScale() const {
        return box.scale;
    }

    vec2 GlyphGeometry::GetBoxTranslate() const {
        return box.translate;
    }

    void GlyphGeometry::GetQuadPlaneBounds(double& l, double& b, double& r, double& t) const {
        if (box.rect.w > 0 && box.rect.h > 0) {
            double invBoxScale = 1 / box.scale;
            l = geometryScale * (-box.translate.x + (box.outerPadding.l + 0.5) * invBoxScale);
            b = geometryScale * (-box.translate.y + (box.outerPadding.b + 0.5) * invBoxScale);
            r = geometryScale * (-box.translate.x + (-box.outerPadding.r + box.rect.w - 0.5) * invBoxScale);
            t = geometryScale * (-box.translate.y + (-box.outerPadding.t + box.rect.h - 0.5) * invBoxScale);
        } else {
            l = 0, b = 0, r = 0, t = 0;
        }
    }

    void GlyphGeometry::GetQuadAtlasBounds(double& l, double& b, double& r, double& t) const {
        if (box.rect.w > 0 && box.rect.h > 0) {
            l = box.rect.x + box.outerPadding.l + 0.5;
            b = box.rect.y + box.outerPadding.b + 0.5;
            r = box.rect.x - box.outerPadding.r + box.rect.w - 0.5;
            t = box.rect.y - box.outerPadding.t + box.rect.h - 0.5;
        } else {
            l = 0, b = 0, r = 0, t = 0;
        }
    }

    bool GlyphGeometry::isWhitespace() const {
        return shape.contours.empty();
    }

    GlyphGeometry::operator GlyphBox() const {
        GlyphBox box;
        box.index = index;
        box.advance = advance;
        GetQuadPlaneBounds(box.bounds.l, box.bounds.b, box.bounds.r, box.bounds.t);
        box.rect.x = this->box.rect.x, box.rect.y = this->box.rect.y, box.rect.w = this->box.rect.w, box.rect.h = this->box.rect.h;
        return box;
    }

    Range operator+(Range a, Range b) {
        return Range(a.lower + b.lower, a.upper + b.upper);
    }
}