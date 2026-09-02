#include <msdf/core/shape/contour.hpp>

// msdf
#include <msdf/core/arithmetic.hpp>
#include <msdf/core/shape/point_bounds.hpp>

namespace qtxt {
    static double shoelace(const vec2& a, const vec2& b) {
        return (b.x - a.x) * (a.y + b.y);
    }

    void Contour::add(const Edge& edge) {
        edges.push_back(edge);
    }

    Edge& Contour::add() {
        edges.resize(edges.size() + 1);
        return edges.back();
    }
    
    void Contour::bound(double& l, double& b, double& r, double& t) const {
        for (const Edge& edge : edges) {
            edge.get()->bound(l, b, r, t);
        }
    }
    void Contour::boundMiters(double& l, double& b, double& r, double& t, double border, double miterLimit, int polarity) const {
        if (edges.empty()) {
            return;
        }
        vec2 prevDir = qbn::norm(edges.back().get()->direction(1), true);
        for (const Edge& edge : edges) {
            vec2 dir = -qbn::norm(edge.get()->direction(0), true);
            if (polarity * qbn::cross(prevDir, dir) >= 0) {
                double miterLength = miterLimit;
                double q = 0.5 * (1 - qbn::dot(prevDir, dir));
                if (q > 0) {
                    miterLength = min(1 / sqrt(q), miterLimit);
                }
                vec2 miter = edge.get()->point(0) + border * miterLength * qbn::norm(prevDir + dir, true);
                pointBounds(miter, l, b, r, t);
            }
            prevDir = qbn::norm(edge.get()->direction(1), true);
        }
    }
    int Contour::winding() const {
        if (edges.empty()) {
            return 0;
        }
        double total = 0;
        if (edges.size() == 1) {
            vec2 a = edges[0].get()->point(0), b = edges[0].get()->point(1 / 3.0), c = edges[0].get()->point(2 / 3.0);
            total += shoelace(a, b);
            total += shoelace(b, c);
            total += shoelace(c, a);
        } else if (edges.size() == 2) {
            vec2 a = edges[0].get()->point(0), b = edges[0].get()->point(0.5), c = edges[1].get()->point(0), d = edges[1].get()->point(0.5);
            total += shoelace(a, b);
            total += shoelace(b, c);
            total += shoelace(c, d);
            total += shoelace(d, a);
        } else {
            vec2 prev = edges.back().get()->point(0);
            for (const Edge& edge : edges) {
                vec2 cur = edge.get()->point(0);
                total += shoelace(prev, cur);
                prev = cur;
            }
        }
        return sign(total);
    }
    void Contour::reverse() {
        for (int i = (int)edges.size() / 2; i > 0; --i) {
            Edge::swap(edges[i - 1], edges[edges.size() - i]);
        }
        for (Edge& edge : edges) {
            edge.get()->reverse();
        }
    }
}