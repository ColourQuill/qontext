#include <msdf/core/shape/cubic_segment.hpp>

// info
#include <info/properties.hpp>

// msdf
#include <msdf/core/arithmetic.hpp>
#include <msdf/core/equation_solver.hpp>
#include <msdf/core/shape/point_bounds.hpp>

namespace qtxt {
    CubicSegment::CubicSegment(vec2 p0, vec2 p1, vec2 p2, vec2 p3, Color color) : Segment(color) {
        p[0] = p0;
        p[1] = p1;
        p[2] = p2;
        p[3] = p3;
    }

    CubicSegment* CubicSegment::clone() const {
        return new CubicSegment(p[0], p[1], p[2], p[3], color);
    }

    EdgeType CubicSegment::type() const {
        return EdgeType::CUBIC;
    }

    const vec2* CubicSegment::control() const {
        return p;
    }

    vec2 CubicSegment::point(double param) const {
        vec2 p12 = mix(p[1], p[2], param);
        return mix(mix(mix(p[0], p[1], param), p12, param), mix(p12, mix(p[2], p[3], param), param), param);
    }

    vec2 CubicSegment::direction(double param) const {
        vec2 tangent = mix(mix(p[1] - p[0], p[2] - p[1], param), mix(p[2] - p[1], p[3] - p[2], param), param);
        if (!tangent) {
            if (param == 0) return p[2] - p[0];
            if (param == 1) return p[3] - p[1];
        }
        return tangent;
    }

    vec2 CubicSegment::directionDelta(double param) const {
        return mix((p[2] - p[1]) - (p[1] - p[0]), (p[3] - p[2]) - (p[2] - p[1]), param);
    }

    SignedDistance CubicSegment::getSignedDistance(vec2 origin, double& param) const {
        vec2 qa = p[0] - origin;
        vec2 ab = p[1] - p[0];
        vec2 br = p[2] - p[1] - ab;
        vec2 as = (p[3] - p[2]) - (p[2] - p[1]) - br;

        vec2 epDir = direction(0);
        double minDistance = nonZeroSign(qbn::cross(epDir, qa)) * qbn::length(qa); // distance from A
        param = -qbn::dot(qa, epDir) / qbn::dot(epDir, epDir);
        {
            epDir = direction(1);
            double distance = qbn::length(p[3] - origin); // distance from B
            if (distance < fabs(minDistance)) {
                minDistance = nonZeroSign(qbn::cross(epDir, p[3] - origin)) * distance;
                param = qbn::dot(epDir-(p[3]-origin), epDir) / qbn::dot(epDir, epDir);
            }
        }
        // Iterative minimum distance search
        for (int i = 0; i <= MSDF_CUBIC_SEARCH_STARTS; ++i) {
            double t = (double) i/MSDF_CUBIC_SEARCH_STARTS;
            vec2 qe = qa + 3 * t * ab + 3 * t * t * br + t * t * t * as;
            for (int step = 0; step < MSDF_CUBIC_SEARCH_STEPS; ++step) {
                // Improve t
                vec2 d1 = 3 * ab + 6 * t * br + 3 * t * t * as;
                vec2 d2 = 6 * br + 6 * t * as;
                t -= qbn::dot(qe, d1) / (qbn::dot(d1, d1) + qbn::dot(qe, d2));
                if (t <= 0 || t >= 1) {
                    break;
                }
                qe = qa + 3 * t * ab + 3 * t * t * br + t * t * t * as;
                double distance = qbn::length(qe);
                if (distance < fabs(minDistance)) {
                    minDistance = nonZeroSign(qbn::cross(d1, qe)) * distance;
                    param = t;
                }
            }
        }

        if (param >= 0 && param <= 1) {
            return SignedDistance(minDistance, 0);
        }
        if (param < 0.5) {
            return SignedDistance(minDistance, fabs(qbn::dot(qbn::norm(direction(0)), qbn::norm(qa))));
        } else {
            return SignedDistance(minDistance, fabs(qbn::dot(qbn::norm(direction(1)), qbn::norm(p[3] - origin))));
        }
    }

    int CubicSegment::scanline(double x[3], int dy[3], double y) const {
        int total = 0;
        int nextDY = y > p[0].y ? 1 : -1;
        x[total] = p[0].x;
        if (p[0].y == y) {
            if (p[0].y < p[1].y || (p[0].y == p[1].y && (p[0].y < p[2].y || (p[0].y == p[2].y && p[0].y < p[3].y))))
                dy[total++] = 1;
            else
                nextDY = 1;
        }
        {
            vec2 ab = p[1] - p[0];
            vec2 br = p[2] - p[1] - ab;
            vec2 as = (p[3] - p[2]) - (p[2] - p[1]) - br;
            double t[3];
            int solutions = solveCubic(t, as.y, 3 * br.y, 3 * ab.y, p[0].y - y);
            // Sort solutions
            double tmp;
            if (solutions >= 2) {
                if (t[0] > t[1])
                    tmp = t[0], t[0] = t[1], t[1] = tmp;
                if (solutions >= 3 && t[1] > t[2]) {
                    tmp = t[1], t[1] = t[2], t[2] = tmp;
                    if (t[0] > t[1])
                        tmp = t[0], t[0] = t[1], t[1] = tmp;
                }
            }
            for (int i = 0; i < solutions && total < 3; ++i) {
                if (t[i] >= 0 && t[i] <= 1) {
                    x[total] = p[0].x + 3 * t[i] * ab.x + 3 * t[i] * t[i] * br.x + t[i] * t[i] * t[i] * as.x;
                    if (nextDY * (ab.y + 2 * t[i] * br.y + t[i] * t[i] * as.y) >= 0) {
                        dy[total++] = nextDY;
                        nextDY = -nextDY;
                    }
                }
            }
        }
        if (p[3].y == y) {
            if (nextDY > 0 && total > 0) {
                --total;
                nextDY = -1;
            }
            if ((p[3].y < p[2].y || (p[3].y == p[2].y && (p[3].y < p[1].y || (p[3].y == p[1].y && p[3].y < p[0].y)))) && total < 3) {
                x[total] = p[3].x;
                if (nextDY < 0) {
                    dy[total++] = -1;
                    nextDY = 1;
                }
            }
        }
        if (nextDY != (y >= p[3].y ? 1 : -1)) {
            if (total > 0)
                --total;
            else {
                if (fabs(p[3].y - y) < fabs(p[0].y - y))
                    x[total] = p[3].x;
                dy[total++] = nextDY;
            }
        }
        return total;
    }

    void CubicSegment::bound(double& l, double& b, double& r, double& t) const {
        pointBounds(p[0], l, b, r, t);
        pointBounds(p[3], l, b, r, t);
        vec2 a0 = p[1] - p[0];
        vec2 a1 = 2 * (p[2] - p[1] - a0);
        vec2 a2 = p[3] - 3 * p[2] + 3 * p[1] - p[0];
        double params[2];
        int solutions;
        solutions = solveQuadratic(params, a2.x, a1.x, a0.x);
        for (int i = 0; i < solutions; ++i)
            if (params[i] > 0 && params[i] < 1) {
                pointBounds(point(params[i]), l, b, r, t);
            }
        solutions = solveQuadratic(params, a2.y, a1.y, a0.y);
        for (int i = 0; i < solutions; ++i)
            if (params[i] > 0 && params[i] < 1) {
                pointBounds(point(params[i]), l, b, r, t);
            }
    }

    void CubicSegment::reverse() {
        vec2 tmp = p[0];
        p[0] = p[3];
        p[3] = tmp;
        tmp = p[1];
        p[1] = p[2];
        p[2] = tmp;
    }

    void CubicSegment::moveStart(vec2 to) {
        p[1] += to - p[0];
        p[0] = to;
    }

    void CubicSegment::moveEnd(vec2 to) {
        p[2] += to - p[3];
        p[3] = to;
    }

    void CubicSegment::split(Segment*& segment0, Segment*& segment1, Segment*& segment2) const {
        segment0 = new CubicSegment(p[0], p[0] == p[1] ? p[0] : mix(p[0], p[1], 1/3.0), mix(mix(p[0], p[1], 1/3.0), mix(p[1], p[2], 1/3.0), 1/3.0), point(1/3.0), color);
        segment1 = new CubicSegment(point(1/3.0),
            mix(mix(mix(p[0], p[1], 1/3.0), mix(p[1], p[2], 1/3.0), 1/3.0), mix(mix(p[1], p[2], 1/3.0), mix(p[2], p[3], 1/3.0), 1/3.0), 2/3.0),
            mix(mix(mix(p[0], p[1], 2/3.0), mix(p[1], p[2], 2/3.0), 2/3.0), mix(mix(p[1], p[2], 2/3.0), mix(p[2], p[3], 2/3.0), 2/3.), 1/3.0),
            point(2/3.0), color);
        segment2 = new CubicSegment(point(2/3.0), mix(mix(p[1], p[2], 2/3.0), mix(p[2], p[3], 2/3.0), 2/3.0), p[2] == p[3] ? p[3] : mix(p[2], p[3], 2/3.0), p[3], color);
    }
}