#ifndef __QONTEXT_CONTOUR_COMBINERS_HPP__
#define __QONTEXT_CONTOUR_COMBINERS_HPP__

// std
#include <vector>

// msdf
#include <msdf/core/distance_selector.hpp>

namespace qtxt {
    // forward declarations
    class Shape;

    class ContourCombiner {
        public:
            explicit ContourCombiner(const Shape&);
            void reset(const vec2& p);
            DistanceSelector& getSelector(int);
            Distance distance() const;
        private:
            DistanceSelector selector;
    };

    class OverlappingContourCombiner {
        public:
            explicit OverlappingContourCombiner(const Shape& shape);
            void reset(const vec2& p);
            DistanceSelector& getSelector(int i);
            Distance distance() const;
        private:
            vec2 p;
            std::vector<int> windings;
            std::vector<DistanceSelector> selectors;
    };
}

#endif