#include <generator/msdf/core/scanline.hpp>

#include <cstdlib>

// msdf
#include <generator/msdf/core/arithmetic.hpp>

namespace qtxt {
    static int CompareIntersections(const void* a, const void* b) {
        return sign(reinterpret_cast<const Intersection*>(a)->x - reinterpret_cast<const Intersection*>(b)->x);
    }

    bool InterpretFillRule(int intersections, FillRule fillRule) {
        switch (fillRule) {
            case FILL_NONZERO:
                return intersections != 0;
            case FILL_ODD:
                return intersections & 1;
            case FILL_POSITIVE:
                return intersections > 0;
            case FILL_NEGATIVE:
                return intersections < 0;
        }
        return false;
    }

    double Scanline::Overlap(const Scanline& a, const Scanline& b, double xFrom, double xTo, FillRule fillRule) {
        double total = 0;
        bool aInside = false, bInside = false;
        int ai = 0, bi = 0;
        double ax = a.numIntersections != 0 ? a.intersections[ai].x : xTo;
        double bx = b.numIntersections != 0 ? b.intersections[bi].x : xTo;
        while (ax < xFrom || bx < xFrom) {
            double xNext = min(ax, bx);
            if (ax == xNext && ai < (int)a.numIntersections) {
                aInside = InterpretFillRule(a.intersections[ai].direction, fillRule);
                ax = ++ai < (int)a.numIntersections ? a.intersections[ai].x : xTo;
            }
            if (bx == xNext && bi < (int)b.numIntersections) {
                bInside = InterpretFillRule(b.intersections[bi].direction, fillRule);
                bx = ++bi < (int)b.numIntersections ? b.intersections[bi].x : xTo;
            }
        }
        double x = xFrom;
        while (ax < xTo || bx < xTo) {
            double xNext = min(ax, bx);
            if (aInside == bInside)
                total += xNext-x;
            if (ax == xNext && ai < (int) a.numIntersections) {
                aInside = InterpretFillRule(a.intersections[ai].direction, fillRule);
                ax = ++ai < (int) a.numIntersections ? a.intersections[ai].x : xTo;
            }
            if (bx == xNext && bi < (int) b.numIntersections) {
                bInside = InterpretFillRule(b.intersections[bi].direction, fillRule);
                bx = ++bi < (int) b.numIntersections ? b.intersections[bi].x : xTo;
            }
            x = xNext;
        }
        if (aInside == bInside)
            total += xTo-x;
        return total;
    }

    Scanline::Scanline() : lastIndex{0} {}

    void Scanline::Preprocess() {
        lastIndex = 0;
        if (numIntersections != 0) {
            qsort(intersections, numIntersections, sizeof(Intersection), CompareIntersections);
            int totalDirection = 0;
            for (int i = 0; i < numIntersections; ++i) {
                totalDirection += intersections[i].direction;
                intersections[i].direction = totalDirection;
            }
        }
    }

    void Scanline::SetIntersections(Intersection* intersections, uint32_t numIntersections) {
        this->numIntersections = numIntersections;
        this->intersections = intersections;
        Preprocess();
    }
    void Scanline::SetIntersections(std::vector<Intersection>& intersections) {
        this->numIntersections = intersections.size();
        this->intersections = intersections.data();
        Preprocess();
    }

    int Scanline::MoveTo(double x) const {
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

    int Scanline::CountIntersections(double x) const {
        return MoveTo(x) + 1;
    }
    int Scanline::SumIntersections(double x) const {
        int index = MoveTo(x);
        if (index >= 0) {
            return intersections[index].direction;
        }
        return 0;
    }
    bool Scanline::Filled(double x, FillRule fillRule) const {
        return InterpretFillRule(SumIntersections(x), fillRule);
    }
}

