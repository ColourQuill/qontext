#ifndef __QONTEXT_SCANLINE_HPP__
#define __QONTEXT_SCANLINE_HPP__

// std
#include <cstdint>
#include <vector>

// msdf
#include <msdf/core/intersection.hpp>

namespace qtxt {
    enum class FillRule {
        NON_ZERO,
        EVEN_ODD,
        POSITIVE,
        NEGATIVE
    };

    bool interpretFillRule(int intersections, FillRule fillRule);

    class Scanline {
        public:
            static double overlap(const Scanline& a, const Scanline& b, double xFrom, double xTo, FillRule fillRule);

            Scanline();

            // Populates intersection array
            void set(Intersection* intersections, uint32_t numIntersections);
            void set(std::vector<Intersection>& intersections);
            // Returns the number of intersections left of x
            int count(double x) const;
            // Returns the total sign of intersections left of x
            int sum(double x) const;
            // Decides whether the scanline is filled at x based on fill rule
            bool filled(double x, FillRule fillRule) const;
        private:
            Intersection* intersections;
            uint32_t numIntersections{0};
            mutable int lastIndex;

            void preprocess();
            int move(double x) const;
    };
}

#endif