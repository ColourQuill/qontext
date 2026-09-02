#include <msdf/core/scanline.hpp>

// std
#include <cstdlib>

// msdf
#include <msdf/core/arithmetic.hpp>

namespace qtxt {
    static int compareIntersections(const void* a, const void* b) {
        return sign(reinterpret_cast<const Intersection*>(a)->x - reinterpret_cast<const Intersection*>(b)->x);
    }
    bool interpretFillRule(int intersections, FillRule fillRule) {
        switch (fillRule) {
            case FillRule::NON_ZERO:
                return intersections != 0;
            case FillRule::EVEN_ODD:
                return intersections & 1;
            case FillRule::POSITIVE:
                return intersections > 0;
            case FillRule::NEGATIVE:
                return intersections < 0;
        }
        return false;
    }

    double Scanline::overlap(const Scanline& a, const Scanline& b, double xFrom, double xTo, FillRule fillRule) {
        double total = 0;
        bool aInside = false, bInside = false;
        int ai = 0, bi = 0;
        double ax = a.numIntersections != 0 ? a.intersections[ai].x : xTo;
        double bx = b.numIntersections != 0 ? b.intersections[bi].x : xTo;
        while (ax < xFrom || bx < xFrom) {
            double xNext = min(ax, bx);
            if (ax == xNext && ai < (int)a.numIntersections) {
                aInside = interpretFillRule(a.intersections[ai].direction, fillRule);
                ax = ++ai < (int)a.numIntersections ? a.intersections[ai].x : xTo;
            }
            if (bx == xNext && bi < (int)b.numIntersections) {
                bInside = interpretFillRule(b.intersections[bi].direction, fillRule);
                bx = ++bi < (int)b.numIntersections ? b.intersections[bi].x : xTo;
            }
        }
        double x = xFrom;
        while (ax < xTo || bx < xTo) {
            double xNext = min(ax, bx);
            if (aInside == bInside)
                total += xNext-x;
            if (ax == xNext && ai < (int) a.numIntersections) {
                aInside = interpretFillRule(a.intersections[ai].direction, fillRule);
                ax = ++ai < (int) a.numIntersections ? a.intersections[ai].x : xTo;
            }
            if (bx == xNext && bi < (int) b.numIntersections) {
                bInside = interpretFillRule(b.intersections[bi].direction, fillRule);
                bx = ++bi < (int) b.numIntersections ? b.intersections[bi].x : xTo;
            }
            x = xNext;
        }
        if (aInside == bInside)
            total += xTo-x;
        return total;
    }

    Scanline::Scanline() : lastIndex{0} {}

    void Scanline::set(Intersection* intersections, uint32_t numIntersections) {
        this->numIntersections = numIntersections;
        this->intersections = intersections;
        preprocess();
    }
    void Scanline::set(std::vector<Intersection>& intersections) {
        this->numIntersections = intersections.size();
        this->intersections = intersections.data();
        preprocess();
    }
    int Scanline::count(double x) const {
        return move(x) + 1;
    }
    int Scanline::sum(double x) const {
        int index = move(x);
        if (index >= 0) {
            return intersections[index].direction;
        }
        return 0;
    }
    bool Scanline::filled(double x, FillRule fillRule) const {
        return interpretFillRule(sum(x), fillRule);
    }

    void Scanline::preprocess() {
        lastIndex = 0;
        if (numIntersections != 0) {
            qsort(intersections, numIntersections, sizeof(Intersection), compareIntersections);
            int totalDirection = 0;
            for (int i = 0; i < numIntersections; ++i) {
                totalDirection += intersections[i].direction;
                intersections[i].direction = totalDirection;
            }
        }
    }
    int Scanline::move(double x) const {
        if (numIntersections == 0) {
            return -1;
        }
        int index = lastIndex;
        if (x < intersections[index].x) {
            do {
                if (index == 0) {
                    lastIndex = 0;
                    return -1;
                }
                --index;
            } while (x < intersections[index].x);
        } else {
            while (index < (int)numIntersections - 1 && x >= intersections[index + 1].x) {
                ++index;
            }
        }
        lastIndex = index;
        return index;
    }
}