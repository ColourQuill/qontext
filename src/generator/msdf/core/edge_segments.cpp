#include <generator/msdf/core/edge_segments.hpp>

#include <generator/msdf/core/arithmetic.hpp>
#include <generator/msdf/core/equation_solver.hpp>

#include <iostream>

namespace qtxt {
    EdgeSegment* EdgeSegment::Create(vec2 p0, vec2 p1, EdgeColor edgeColor) {
        return new LinearSegment(p0, p1, edgeColor);
    }

    EdgeSegment* EdgeSegment::Create(vec2 p0, vec2 p1, vec2 p2, EdgeColor edgeColor) {
        if (!qbn::cross(p1 - p0, p2 - p1)) {
            return new LinearSegment(p0, p2, edgeColor);
        }
        return new QuadraticSegment(p0, p1, p2, edgeColor);
    }

    EdgeSegment* EdgeSegment::Create(vec2 p0, vec2 p1, vec2 p2, vec2 p3, EdgeColor edgeColor) {
        vec2 p12 = p2 - p1;
        if (!qbn::cross(p1 - p0, p12) && !qbn::cross(p12, p3 - p2)) {
            return new LinearSegment(p0, p3, edgeColor);
        }
        if ((p12 = 1.5 * p1 -0.5 * p0) == 1.5 * p2 -0.5 * p3) {
            return new QuadraticSegment(p0, p12, p3, edgeColor);
        }
        return new CubicSegment(p0, p1, p2, p3, edgeColor);
    }

    void EdgeSegment::DistanceToPerpendicularDistance(SignedDistance& distance, vec2 origin, double param) const {
        if (param < 0) {
            vec2 dir = qbn::norm(Direction(0));
            vec2 aq = origin - Point(0);
            double ts = qbn::dot(aq, dir);
            if (ts < 0) {
                double perpendicularDistance = qbn::cross(aq, dir);
                if (fabs(perpendicularDistance) <= fabs(distance.distance)) {
                    distance.distance = perpendicularDistance;
                    distance.dot = 0;
                }
            }
        } else if (param > 1) {
            vec2 dir = qbn::norm(Direction(1));
            vec2 bq = origin - Point(1);
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

    LinearSegment::LinearSegment(vec2 p0, vec2 p1, EdgeColor edgeColor) : EdgeSegment(edgeColor) {
        p[0] = p0;
        p[1] = p1;
    }

    QuadraticSegment::QuadraticSegment(vec2 p0, vec2 p1, vec2 p2, EdgeColor edgeColor) : EdgeSegment(edgeColor) {
        p[0] = p0;
        p[1] = p1;
        p[2] = p2;
    }

    CubicSegment::CubicSegment(vec2 p0, vec2 p1, vec2 p2, vec2 p3, EdgeColor edgeColor) : EdgeSegment(edgeColor) {
        p[0] = p0;
        p[1] = p1;
        p[2] = p2;
        p[3] = p3;
    }

    LinearSegment* LinearSegment::Clone() const {
        return new LinearSegment(p[0], p[1], color);
    }

    QuadraticSegment* QuadraticSegment::Clone() const {
        return new QuadraticSegment(p[0], p[1], p[2], color);
    }

    CubicSegment* CubicSegment::Clone() const {
        return new CubicSegment(p[0], p[1], p[2], p[3], color);
    }

    int LinearSegment::Type() const {
        return (int)EDGE_TYPE;
    }

    int QuadraticSegment::Type() const {
        return (int)EDGE_TYPE;
    }

    int CubicSegment::Type() const {
        return (int)EDGE_TYPE;
    }

    const vec2* LinearSegment::ControlPoints() const {
        return p;
    }

    const vec2* QuadraticSegment::ControlPoints() const {
        return p;
    }

    const vec2* CubicSegment::ControlPoints() const {
        return p;
    }

    vec2 LinearSegment::Point(double param) const {
        return mix(p[0], p[1], param);
    }

    vec2 QuadraticSegment::Point(double param) const {
        return mix(mix(p[0], p[1], param), mix(p[1], p[2], param), param);
    }

    vec2 CubicSegment::Point(double param) const {
        vec2 p12 = mix(p[1], p[2], param);
        return mix(mix(mix(p[0], p[1], param), p12, param), mix(p12, mix(p[2], p[3], param), param), param);
    }

    vec2 LinearSegment::Direction(double param) const {
        return p[1]-p[0];
    }

    vec2 QuadraticSegment::Direction(double param) const {
        vec2 tangent = mix(p[1] - p[0], p[2] - p[1], param);
        if (!tangent) {
            return p[2]-p[0];
        }
        return tangent;
    }

    vec2 CubicSegment::Direction(double param) const {
        vec2 tangent = mix(mix(p[1]-p[0], p[2]-p[1], param), mix(p[2]-p[1], p[3]-p[2], param), param);
        if (!tangent) {
            if (param == 0) return p[2]-p[0];
            if (param == 1) return p[3]-p[1];
        }
        return tangent;
    }

    vec2 LinearSegment::DirectionChange(double param) const {
        return vec2();
    }

    vec2 QuadraticSegment::DirectionChange(double param) const {
        return (p[2] - p[1]) - (p[1] - p[0]);
    }

    vec2 CubicSegment::DirectionChange(double param) const {
        return mix((p[2] - p[1]) - (p[1] - p[0]), (p[3] - p[2]) - (p[2] - p[1]), param);
    }

    double LinearSegment::Length() const {
        return qbn::length(p[1] - p[0]);
    }

    double QuadraticSegment::Length() const {
        vec2 ab = p[1] - p[0];
        vec2 br = p[2] - p[1] - ab;
        double abab = qbn::dot(ab, ab);
        double abbr = qbn::dot(ab, br);
        double brbr = qbn::dot(br, br);
        double abLen = sqrt(abab);
        double brLen = sqrt(brbr);
        double crs = qbn::cross(ab, br);
        double h = sqrt(abab+abbr+abbr+brbr);
        return (
            brLen * ((abbr + brbr) * h - abbr * abLen) +
            crs * crs * log((brLen * h + abbr + brbr) / (brLen * abLen + abbr))
        ) / (brbr*brLen);
    }

    SignedDistance LinearSegment::GetSignedDistance(vec2 origin, double &param) const {
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

    SignedDistance QuadraticSegment::GetSignedDistance(vec2 origin, double &param) const {
        vec2 qa = p[0] - origin;
        vec2 ab = p[1]-p[0];
        vec2 br = p[2] - p[1] - ab;
        double a = qbn::dot(br, br);
        double b = 3 * qbn::dot(ab, br);
        double c = 2 * qbn::dot(ab, ab) + qbn::dot(qa, br);
        double d = qbn::dot(qa, ab);
        double t[3];
        int solutions = SolveCubic(t, a, b, c, d);

        vec2 epDir = Direction(0);
        double minDistance = nonZeroSign(qbn::cross(epDir, qa)) * qbn::length(qa); // distance from A
        param = -qbn::dot(qa, epDir) / qbn::dot(epDir, epDir);
        {
            epDir = Direction(1);
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
            return SignedDistance(minDistance, fabs(qbn::dot(qbn::norm(Direction(0)), qbn::norm(qa))));
        } else {
            return SignedDistance(minDistance, fabs(qbn::dot(qbn::norm(Direction(1)), qbn::norm((p[2]-origin)))));
        }
    }

    SignedDistance CubicSegment::GetSignedDistance(vec2 origin, double &param) const {
        vec2 qa = p[0]-origin;
        vec2 ab = p[1]-p[0];
        vec2 br = p[2]-p[1]-ab;
        vec2 as = (p[3]-p[2])-(p[2]-p[1])-br;

        vec2 epDir = Direction(0);
        double minDistance = nonZeroSign(qbn::cross(epDir, qa)) * qbn::length(qa); // distance from A
        param = -qbn::dot(qa, epDir) / qbn::dot(epDir, epDir);
        {
            epDir = Direction(1);
            double distance = qbn::length(p[3] - origin); // distance from B
            if (distance < fabs(minDistance)) {
                minDistance = nonZeroSign(qbn::cross(epDir, p[3] - origin)) * distance;
                param = qbn::dot(epDir-(p[3]-origin), epDir) / qbn::dot(epDir, epDir);
            }
        }
        // Iterative minimum distance search
        for (int i = 0; i <= MSDF_CUBIC_SEARCH_STARTS; ++i) {
            double t = (double) i/MSDF_CUBIC_SEARCH_STARTS;
            vec2 qe = qa+3*t*ab+3*t*t*br+t*t*t*as;
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
            return SignedDistance(minDistance, fabs(qbn::dot(qbn::norm(Direction(0)), qbn::norm(qa))));
        } else {
            return SignedDistance(minDistance, fabs(qbn::dot(qbn::norm(Direction(1)), qbn::norm(p[3] - origin))));
        }
    }

    int LinearSegment::ScanlineIntersections(double x[3], int dy[3], double y) const {
        if ((y >= p[0].y && y < p[1].y) || (y >= p[1].y && y < p[0].y)) {
            double param = (y - p[0].y) / (p[1].y - p[0].y);
            x[0] = mix(p[0].x, p[1].x, param);
            dy[0] = sign(p[1].y - p[0].y);
            return 1;
        }
        return 0;
    }

    int QuadraticSegment::ScanlineIntersections(double x[3], int dy[3], double y) const {
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
            int solutions = SolveQuadratic(t, br.y, 2 * ab.y, p[0].y - y);
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

    int CubicSegment::ScanlineIntersections(double x[3], int dy[3], double y) const {
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
            int solutions = SolveCubic(t, as.y, 3 * br.y, 3 * ab.y, p[0].y - y);
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

    static void PointBounds(vec2 p, double &l, double &b, double &r, double &t) {
        if (p.x < l) l = p.x;
        if (p.y < b) b = p.y;
        if (p.x > r) r = p.x;
        if (p.y > t) t = p.y;
    }

    void LinearSegment::Bound(double &l, double &b, double &r, double &t) const {
        PointBounds(p[0], l, b, r, t);
        PointBounds(p[1], l, b, r, t);
    }

    void QuadraticSegment::Bound(double &l, double &b, double &r, double &t) const {
        PointBounds(p[0], l, b, r, t);
        PointBounds(p[2], l, b, r, t);
        vec2 bot = (p[1] - p[0]) - (p[2] - p[1]);
        if (bot.x) {
            double param = (p[1].x-p[0].x)/bot.x;
            if (param > 0 && param < 1)
                PointBounds(Point(param), l, b, r, t);
        }
        if (bot.y) {
            double param = (p[1].y-p[0].y)/bot.y;
            if (param > 0 && param < 1)
                PointBounds(Point(param), l, b, r, t);
        }
    }

    void CubicSegment::Bound(double &l, double &b, double &r, double &t) const {
        PointBounds(p[0], l, b, r, t);
        PointBounds(p[3], l, b, r, t);
        vec2 a0 = p[1] - p[0];
        vec2 a1 = 2 * (p[2] - p[1] - a0);
        vec2 a2 = p[3] - 3 * p[2] + 3 * p[1] - p[0];
        double params[2];
        int solutions;
        solutions = SolveQuadratic(params, a2.x, a1.x, a0.x);
        for (int i = 0; i < solutions; ++i)
            if (params[i] > 0 && params[i] < 1) {
                PointBounds(Point(params[i]), l, b, r, t);
            }
        solutions = SolveQuadratic(params, a2.y, a1.y, a0.y);
        for (int i = 0; i < solutions; ++i)
            if (params[i] > 0 && params[i] < 1) {
                PointBounds(Point(params[i]), l, b, r, t);
            }
    }

    void LinearSegment::Reverse() {
        vec2 tmp = p[0];
        p[0] = p[1];
        p[1] = tmp;
    }

    void QuadraticSegment::Reverse() {
        vec2 tmp = p[0];
        p[0] = p[2];
        p[2] = tmp;
    }

    void CubicSegment::Reverse() {
        vec2 tmp = p[0];
        p[0] = p[3];
        p[3] = tmp;
        tmp = p[1];
        p[1] = p[2];
        p[2] = tmp;
    }

    void LinearSegment::MoveStartPoint(vec2 to) {
        p[0] = to;
    }

    void QuadraticSegment::MoveStartPoint(vec2 to) {
        vec2 origSDir = p[0] - p[1];
        vec2 origP1 = p[1];
        p[1] += qbn::cross(p[0] - p[1], to - p[0]) / qbn::cross(p[0] - p[1], p[2] - p[1]) * (p[2] - p[1]);
        p[0] = to;
        if (qbn::dot(origSDir, p[0] - p[1]) < 0) {
            p[1] = origP1;
        }
    }

    void CubicSegment::MoveStartPoint(vec2 to) {
        p[1] += to - p[0];
        p[0] = to;
    }

    void LinearSegment::MoveEndPoint(vec2 to) {
        p[1] = to;
    }

    void QuadraticSegment::MoveEndPoint(vec2 to) {
        vec2 origEDir = p[2] - p[1];
        vec2 origP1 = p[1];
        p[1] += qbn::cross(p[2] - p[1], to-p[2]) / qbn::cross(p[2] - p[1], p[0] - p[1]) * (p[0] - p[1]);
        p[2] = to;
        if (qbn::dot(origEDir, p[2]-p[1]) < 0) {
            p[1] = origP1;
        }
    }

    void CubicSegment::MoveEndPoint(vec2 to) {
        p[2] += to - p[3];
        p[3] = to;
    }

    void LinearSegment::SplitInThirds(EdgeSegment*& part0, EdgeSegment*& part1, EdgeSegment*& part2) const {
        part0 = new LinearSegment(p[0], Point(1/3.0), color);
        part1 = new LinearSegment(Point(1/3.0), Point(2/3.0), color);
        part2 = new LinearSegment(Point(2/3.0), p[1], color);
    }

    void QuadraticSegment::SplitInThirds(EdgeSegment*& part0, EdgeSegment*& part1, EdgeSegment*& part2) const {
        part0 = new QuadraticSegment(p[0], mix(p[0], p[1], 1/3.0), Point(1/3.0), color);
        part1 = new QuadraticSegment(Point(1/3.0), mix(mix(p[0], p[1], 5/9.0), mix(p[1], p[2], 4/9.0), 0.5), Point(2/3.0), color);
        part2 = new QuadraticSegment(Point(2/3.0), mix(p[1], p[2], 2/3.0), p[2], color);
    }

    void CubicSegment::SplitInThirds(EdgeSegment*& part0, EdgeSegment*& part1, EdgeSegment*& part2) const {
        part0 = new CubicSegment(p[0], p[0] == p[1] ? p[0] : mix(p[0], p[1], 1/3.0), mix(mix(p[0], p[1], 1/3.0), mix(p[1], p[2], 1/3.0), 1/3.0), Point(1/3.0), color);
        part1 = new CubicSegment(Point(1/3.0),
            mix(mix(mix(p[0], p[1], 1/3.0), mix(p[1], p[2], 1/3.0), 1/3.0), mix(mix(p[1], p[2], 1/3.0), mix(p[2], p[3], 1/3.0), 1/3.0), 2/3.0),
            mix(mix(mix(p[0], p[1], 2/3.0), mix(p[1], p[2], 2/3.0), 2/3.0), mix(mix(p[1], p[2], 2/3.0), mix(p[2], p[3], 2/3.0), 2/3.), 1/3.0),
            Point(2/3.0), color);
        part2 = new CubicSegment(Point(2/3.0), mix(mix(p[1], p[2], 2/3.0), mix(p[2], p[3], 2/3.0), 2/3.0), p[2] == p[3] ? p[3] : mix(p[2], p[3], 2/3.0), p[3], color);
    }

    EdgeSegment* QuadraticSegment::ConvertToCubic() const {
        return new CubicSegment(p[0], mix(p[0], p[1], 2/3.), mix(p[1], p[2], 1/3.), p[2], color);
    }
}