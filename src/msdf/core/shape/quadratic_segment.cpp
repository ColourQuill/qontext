#include <msdf/core/shape/quadratic_segment.hpp>

// msdf
#include <msdf/core/arithmetic.hpp>
#include <msdf/core/equation_solver.hpp>
#include <msdf/core/shape/point_bounds.hpp>
#include <msdf/core/shape/cubic_segment.hpp>

namespace qtxt {
    QuadraticSegment::QuadraticSegment(vec2 p0, vec2 p1, vec2 p2, Color color) : Segment(color) {
        p[0] = p0;
        p[1] = p1;
        p[2] = p2;
    }

    QuadraticSegment* QuadraticSegment::clone() const {
        return new QuadraticSegment(p[0], p[1], p[2], color);
    }

    EdgeType QuadraticSegment::type() const {
        return EdgeType::QUADRATIC;
    }
    
    const vec2* QuadraticSegment::control() const {
        return p;
    }

    vec2 QuadraticSegment::point(double param) const {
        return mix(mix(p[0], p[1], param), mix(p[1], p[2], param), param);
    }

    vec2 QuadraticSegment::direction(double param) const {
        vec2 tangent = mix(p[1] - p[0], p[2] - p[1], param);
        if (!tangent) {
            return p[2]-p[0];
        }
        return tangent;
    }

    vec2 QuadraticSegment::directionDelta(double param) const {
        return (p[2] - p[1]) - (p[1] - p[0]);
    }

    double QuadraticSegment::length() const {
        vec2 ab = p[1] - p[0];
        vec2 br = p[2] - p[1] - ab;
        double abab = qbn::dot(ab, ab);
        double abbr = qbn::dot(ab, br);
        double brbr = qbn::dot(br, br);
        double abLen = sqrt(abab);
        double brLen = sqrt(brbr);
        double crs = qbn::cross(ab, br);
        double h = sqrt(abab+abbr+abbr+brbr);
        return (brLen * ((abbr + brbr) * h - abbr * abLen) + crs * crs * log((brLen * h + abbr + brbr) / (brLen * abLen + abbr))) / (brbr*brLen);
    }

    SignedDistance QuadraticSegment::getSignedDistance(vec2 origin, double& param) const {
        vec2 qa = p[0] - origin;
        vec2 ab = p[1]-p[0];
        vec2 br = p[2] - p[1] - ab;
        double a = qbn::dot(br, br);
        double b = 3 * qbn::dot(ab, br);
        double c = 2 * qbn::dot(ab, ab) + qbn::dot(qa, br);
        double d = qbn::dot(qa, ab);
        double t[3];
        int solutions = solveCubic(t, a, b, c, d);

        vec2 epDir = direction(0);
        double minDistance = nonZeroSign(qbn::cross(epDir, qa)) * qbn::length(qa); // distance from A
        param = -qbn::dot(qa, epDir) / qbn::dot(epDir, epDir);
        {
            epDir = direction(1);
            double distance = qbn::length(p[2] - origin); // distance from B
            if (distance < fabs(minDistance)) {
                minDistance = nonZeroSign(qbn::cross(epDir, p[2] - origin)) * distance;
                param = qbn::dot(origin-p[1], epDir) / qbn::dot(epDir, epDir);
            }
        }
        for (int i = 0; i < solutions; ++i) {
            if (t[i] > 0 && t[i] < 1) {
                vec2 qe = qa + 2 * t[i] * ab + t[i] * t[i] * br;
                double distance = qbn::length(qe);
                if (distance <= fabs(minDistance)) {
                    minDistance = nonZeroSign(qbn::cross(ab + t[i] * br, qe)) * distance;
                    param = t[i];
                }
            }
        }

        if (param >= 0 && param <= 1) {
            return SignedDistance(minDistance, 0);
        } 
        if (param < .5) {
            return SignedDistance(minDistance, fabs(qbn::dot(qbn::norm(direction(0)), qbn::norm(qa))));
        } else {
            return SignedDistance(minDistance, fabs(qbn::dot(qbn::norm(direction(1)), qbn::norm((p[2] - origin)))));
        }
    }

    int QuadraticSegment::scanline(double x[3], int dy[3], double y) const {
        int total = 0;
        int nextDY = y > p[0].y ? 1 : -1;
        x[total] = p[0].x;
        if (p[0].y == y) {
            if (p[0].y < p[1].y || (p[0].y == p[1].y && p[0].y < p[2].y))
                dy[total++] = 1;
            else
                nextDY = 1;
        }
        {
            vec2 ab = p[1] - p[0];
            vec2 br = p[2] - p[1]-ab;
            double t[2];
            int solutions = solveQuadratic(t, br.y, 2 * ab.y, p[0].y - y);
            // Sort solutions
            double tmp;
            if (solutions >= 2 && t[0] > t[1])
                tmp = t[0], t[0] = t[1], t[1] = tmp;
            for (int i = 0; i < solutions && total < 2; ++i) {
                if (t[i] >= 0 && t[i] <= 1) {
                    x[total] = p[0].x + 2 * t[i] * ab.x + t[i] * t[i] * br.x;
                    if (nextDY*(ab.y+t[i]*br.y) >= 0) {
                        dy[total++] = nextDY;
                        nextDY = -nextDY;
                    }
                }
            }
        }
        if (p[2].y == y) {
            if (nextDY > 0 && total > 0) {
                --total;
                nextDY = -1;
            }
            if ((p[2].y < p[1].y || (p[2].y == p[1].y && p[2].y < p[0].y)) && total < 2) {
                x[total] = p[2].x;
                if (nextDY < 0) {
                    dy[total++] = -1;
                    nextDY = 1;
                }
            }
        }
        if (nextDY != (y >= p[2].y ? 1 : -1)) {
            if (total > 0)
                --total;
            else {
                if (fabs(p[2].y-y) < fabs(p[0].y-y))
                    x[total] = p[2].x;
                dy[total++] = nextDY;
            }
        }
        return total;
    }

    void QuadraticSegment::bound(double &l, double &b, double &r, double &t) const {
        pointBounds(p[0], l, b, r, t);
        pointBounds(p[2], l, b, r, t);
        vec2 bot = (p[1] - p[0]) - (p[2] - p[1]);
        if (bot.x) {
            double param = (p[1].x-p[0].x)/bot.x;
            if (param > 0 && param < 1)
                pointBounds(point(param), l, b, r, t);
        }
        if (bot.y) {
            double param = (p[1].y-p[0].y)/bot.y;
            if (param > 0 && param < 1)
                pointBounds(point(param), l, b, r, t);
        }
    }

    void QuadraticSegment::reverse() {
        vec2 tmp = p[0];
        p[0] = p[2];
        p[2] = tmp;
    }

    void QuadraticSegment::moveStart(vec2 to) {
        vec2 origSDir = p[0] - p[1];
        vec2 origP1 = p[1];
        p[1] += qbn::cross(p[0] - p[1], to - p[0]) / qbn::cross(p[0] - p[1], p[2] - p[1]) * (p[2] - p[1]);
        p[0] = to;
        if (qbn::dot(origSDir, p[0] - p[1]) < 0) {
            p[1] = origP1;
        }
    }

    void QuadraticSegment::moveEnd(vec2 to) {
        vec2 origEDir = p[2] - p[1];
        vec2 origP1 = p[1];
        p[1] += qbn::cross(p[2] - p[1], to-p[2]) / qbn::cross(p[2] - p[1], p[0] - p[1]) * (p[0] - p[1]);
        p[2] = to;
        if (qbn::dot(origEDir, p[2]-p[1]) < 0) {
            p[1] = origP1;
        }
    }

    void QuadraticSegment::split(Segment*& segment0, Segment*& segment1, Segment*& segment2) const {
        segment0 = new QuadraticSegment(p[0], mix(p[0], p[1], 1 / 3.0), point(1 / 3.0), color);
        segment1 = new QuadraticSegment(point(1 / 3.0), mix(mix(p[0], p[1], 5 / 9.0), mix(p[1], p[2], 4 / 9.0), 0.5), point(2 / 3.0), color);
        segment2 = new QuadraticSegment(point(2 / 3.0), mix(p[1], p[2], 2 / 3.0), p[2], color);
    }

    Segment* QuadraticSegment::convert() const {
        return new CubicSegment(p[0], mix(p[0], p[1], 2 / 3.0), mix(p[1], p[2], 1 / 3.0), p[2], color);
    }
}