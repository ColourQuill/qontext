#ifndef __QONTEXT_SCANLINE_HPP__
#define __QONTEXT_SCANLINE_HPP__

// std
#include <cstdint>
#include <vector>

// msdf
#include <generator/msdf/core/intersection.hpp>

namespace qtxt {
    enum FillRule {
        FILL_NONZERO,
        FILL_ODD, // "even-odd"
        FILL_POSITIVE,
        FILL_NEGATIVE
    };

    bool InterpretFillRule(int intersections, FillRule fillRule);

    class Scanline {
        public:
            static double Overlap(const Scanline& a, const Scanline& b, double xFrom, double xTo, FillRule fillRule);

            Scanline();

            // Populates intersection array
            void SetIntersections(Intersection* intersections, uint32_t numIntersections);
            void SetIntersections(std::vector<Intersection>& intersections);
            // Returns the number of intersections left of x
            int CountIntersections(double x) const;
            // Returns the total sign of intersections left of x
            int SumIntersections(double x) const;
            // Decides whether the scanline is filled at x based on fill rule
            bool Filled(double x, FillRule fillRule) const;
            
        private:
            Intersection* intersections;
            uint32_t numIntersections{0};
            mutable int lastIndex;

            void Preprocess();
            int MoveTo(double x) const;
    };
}

#endif