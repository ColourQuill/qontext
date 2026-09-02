#include <msdf/core/projection.hpp>

namespace qtxt {
    Projection::Projection() : scale{vec2{1, 1}}, translate{vec2{0, 0}} { }

    Projection::Projection(const vec2& scale, const vec2& translate) : scale{scale}, translate{translate} { }

    vec2 Projection::project(const vec2& coord) const {
        return scale * (coord + translate);
    }

    vec2 Projection::unproject(const vec2& coord) const {
        return coord / scale - translate;
    }

    double Projection::projectX(double x) const {
        return scale.x * (x + translate.x);
    }

    double Projection::projectY(double y) const {
        return scale.y * (y + translate.y);
    }

    double Projection::unprojectX(double x) const {
        return x / scale.x - translate.x;
    }

    double Projection::unprojectY(double y) const {
        return y / scale.y - translate.y;
    }
}