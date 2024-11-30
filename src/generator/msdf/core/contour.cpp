#include <generator/msdf/core/contour.hpp>

// qarbon
#include <qarbon.hpp>

// msdf
#include <generator/msdf/core/arithmetic.hpp>

using vec2 = qbn::vec2<double>;

namespace qtxt {
    static double shoelace(const vec2& a, const vec2& b) {
        return (b.x - a.x) * (a.y + b.y);
    }
    static void BoundPoint(double& l, double& b, double& r, double& t, vec2 p) {
        if (p.x < l) { l = p.x; }
        if (p.y < b) { b = p.y; }
        if (p.x > r) { r = p.x; }
        if (p.y > t) { t = p.y; }
    }
    void Contour::AddEdge(const EdgeHolder& edge) {
        edges.push_back(edge);
    }

    EdgeHolder& Contour::AddEdge() {
        edges.resize(edges.size() + 1);
        return edges.back();
    }

    void Contour::Bound(double& l, double& b, double& r, double& t) const {
        for (const EdgeHolder& edge : edges) {
            edge->Bound(l, b, r, t);
        }
    }
    void Contour::BoundMiters(double& l, double& b, double& r, double& t, double border, double miterLimit, int polarity) const {
        if (edges.empty()) {
            return;
        }
        vec2 prevDir = qbn::norm(edges.back()->Direction(1), true);
        for (const EdgeHolder& edge : edges) {
            vec2 dir = -qbn::norm(edge->Direction(0), true);
            if (polarity * qbn::cross(prevDir, dir) >= 0) {
                double miterLength = miterLimit;
                double q = 0.5 * (1 - qbn::dot(prevDir, dir));
                if (q > 0) {
                    miterLength = min(1 / sqrt(q), miterLimit);
                }
                vec2 miter = edge->Point(0) + border * miterLength * qbn::norm(prevDir + dir, true);
                BoundPoint(l, b, r, t, miter);
            }
            prevDir = qbn::norm(edge->Direction(1), true);
        }
    }
    int Contour::Winding() const {
        if (edges.empty()) {
            return 0;
        }
        double total = 0;
        if (edges.size() == 1) {
            vec2 a = edges[0]->Point(0), b = edges[0]->Point(1/3.0), c = edges[0]->Point(2/3.0);
            total += shoelace(a, b);
            total += shoelace(b, c);
            total += shoelace(c, a);
        } else if (edges.size() == 2) {
            vec2 a = edges[0]->Point(0), b = edges[0]->Point(0.5), c = edges[1]->Point(0), d = edges[1]->Point(0.5);
            total += shoelace(a, b);
            total += shoelace(b, c);
            total += shoelace(c, d);
            total += shoelace(d, a);
        } else {
            vec2 prev = edges.back()->Point(0);
            for (const EdgeHolder& edge : edges) {
                vec2 cur = edge->Point(0);
                total += shoelace(prev, cur);
                prev = cur;
            }
        }
        return sign(total);
    }
    void Contour::Reverse() {
        for (int i = (int)edges.size() / 2; i > 0; --i) {
            EdgeHolder::Swap(edges[i - 1], edges[edges.size() - i]);
        }
        for (EdgeHolder& edge : edges) {
            edge->Reverse();
        }
    }
}