#include <msdf/core/shape/linear_segment.hpp>

// msdf
#include <msdf/core/arithmetic.hpp>
#include <msdf/core/shape/point_bounds.hpp>

namespace qtxt {
    LinearSegment::LinearSegment(vec2 p0, vec2 p1, Color color) : Segment(color) {
        p[0] = p0;
        p[1] = p1;
    }

    LinearSegment* LinearSegment::clone() const {
        return new LinearSegment(p[0], p[1], color);
    }

    EdgeType LinearSegment::type() const {
        return EdgeType::LINEAR;
    }

    const vec2* LinearSegment::control() const {
        return p;
    }

    vec2 LinearSegment::point(double param) const {
        return mix(p[0], p[1], param);
    }

    vec2 LinearSegment::direction(double param) const {
        return p[1] - p[0];
    }

    vec2 LinearSegment::directionDelta(double param) const {
        return vec2();
    }

    double LinearSegment::length() const {
        return qbn::length(p[1] - p[0]);
    }

    SignedDistance LinearSegment::getSignedDistance(vec2 origin, double& param) const {
        vec2 aq = origin - p[0];
        vec2 ab = p[1] - p[0];
        param = qbn::dot(aq, ab) / qbn::dot(ab, ab);
        vec2 eq = p[param > 0.5]-origin;
        double endpointDistance = qbn::length(eq);
        if (param > 0 && param < 1) {
            double orthoDistance = qbn::dot(qbn::orthonormal2D(ab, false), aq);
            if (fabs(orthoDistance) < endpointDistance) {
                return SignedDistance(orthoDistance, 0);
            }
        }
        return SignedDistance(nonZeroSign(qbn::cross(aq, ab)) * endpointDistance, fabs(qbn::dot(qbn::norm(ab), qbn::norm(eq))));
    }

    int LinearSegment::scanline(double x[3], int dy[3], double y) const {
        if ((y >= p[0].y && y < p[1].y) || (y >= p[1].y && y < p[0].y)) {
            double param = (y - p[0].y) / (p[1].y - p[0].y);
            x[0] = mix(p[0].x, p[1].x, param);
            dy[0] = sign(p[1].y - p[0].y);
            return 1;
        }
        return 0;
    }

    void LinearSegment::bound(double& l, double& b, double& r, double& t) const {
        pointBounds(p[0], l, b, r, t);
        pointBounds(p[1], l, b, r, t);
    }

    void LinearSegment::reverse() {
        vec2 tmp = p[0];
        p[0] = p[1];
        p[1] = tmp;
    }

    void LinearSegment::moveStart(vec2 to) {
        p[0] = to;
    }

    void LinearSegment::moveEnd(vec2 to) {
        p[1] = to;
    }

    void LinearSegment::split(Segment*& segment0, Segment*& segment1, Segment*& segment2) const {
        segment0 = new LinearSegment(p[0], point(1 / 3.0), color);
        segment1 = new LinearSegment(point(1 / 3.0), point(2 / 3.0), color);
        segment2 = new LinearSegment(point(2 / 3.0), p[1], color);
    }
}