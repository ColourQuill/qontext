#include <msdf/core/shape/segment.hpp>

// qarbon
#include <qarbon.hpp>

// msdf
#include <msdf/core/shape/linear_segment.hpp>
#include <msdf/core/shape/quadratic_segment.hpp>
#include <msdf/core/shape/cubic_segment.hpp>

namespace qtxt {
    Segment* Segment::create(vec2 p0, vec2 p1, Color color) {
        return new LinearSegment(p0, p1, color);
    }

    Segment* Segment::create(vec2 p0, vec2 p1, vec2 p2, Color color) {
        if (!qbn::cross(p1 - p0, p2 - p1)) {
            return new LinearSegment(p0, p2, color);
        }
        return new QuadraticSegment(p0, p1, p2, color);
    }

    Segment* Segment::create(vec2 p0, vec2 p1, vec2 p2, vec2 p3, Color color) {
        vec2 p12 = p2 - p1;
        if (!qbn::cross(p1 - p0, p12) && !qbn::cross(p12, p3 - p2)) {
            return new LinearSegment(p0, p3, color);
        }
        if ((p12 = 1.5 * p1 -0.5 * p0) == 1.5 * p2 -0.5 * p3) {
            return new QuadraticSegment(p0, p12, p3, color);
        }
        return new CubicSegment(p0, p1, p2, p3, color);
    }

    void Segment::distanceToPerpendicularDistance(SignedDistance& distance, vec2 origin, double param) const {
        if (param < 0) {
            vec2 dir = qbn::norm(direction(0));
            vec2 aq = origin - point(0);
            double ts = qbn::dot(aq, dir);
            if (ts < 0) {
                double perpendicularDistance = qbn::cross(aq, dir);
                if (fabs(perpendicularDistance) <= fabs(distance.distance)) {
                    distance.distance = perpendicularDistance;
                    distance.dot = 0;
                }
            }
        } else if (param > 1) {
            vec2 dir = qbn::norm(direction(1));
            vec2 bq = origin - point(1);
            double ts = qbn::dot(bq, dir);
            if (ts > 0) {
                double perpendicularDistance = qbn::cross(bq, dir);
                if (fabs(perpendicularDistance) <= fabs(distance.distance)) {
                    distance.distance = perpendicularDistance;
                    distance.dot = 0;
                }
            }
        }
    }
}