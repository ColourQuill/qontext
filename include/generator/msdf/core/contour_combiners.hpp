#ifndef __QONTEXT_CONTOUR_COMBINERS_HPP__
#define __QONTEXT_CONTOUR_COMBINERS_HPP__

// std
#include <vector>

// msdf
#include <generator/msdf/core/edge_selectors.hpp>

namespace qtxt {
    // forward declarations
    class Shape;

    template<class EdgeSelector>
    class SimpleContourCombiner {
        public:
            typedef EdgeSelector EdgeSelectorType;
            typedef typename EdgeSelector::DistanceType DistanceType;

            explicit SimpleContourCombiner(const Shape& shape);
            void Reset(const vec2& p);
            EdgeSelector& GetEdgeSelector(int i);
            DistanceType Distance() const;
        private:
            EdgeSelector shapeEdgeSelector;
    };

    template<class EdgeSelector>
    class OverlappingContourCombiner {
        public:
            typedef EdgeSelector EdgeSelectorType;
            typedef typename EdgeSelector::DistanceType DistanceType;

            explicit OverlappingContourCombiner(const Shape& shape);
            void Reset(const vec2& p);
            EdgeSelector& GetEdgeSelector(int i );
            DistanceType Distance() const;
        private:
            vec2 p;
            std::vector<int> windings;
            std::vector<EdgeSelector> edgeSelectors;
    };
}

#endif