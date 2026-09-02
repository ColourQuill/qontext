#ifndef __QONTEXT_SHAPE_DISTANCE_FINDER_HPP__
#define __QONTEXT_SHAPE_DISTANCE_FINDER_HPP__

// std
#include <vector>

// msdf
#include <msdf/core/shape/shape.hpp>
#include <msdf/core/shape/contour.hpp>
#include <msdf/core/distance_selector.hpp>
#include <msdf/core/contour_combiners.hpp>

namespace qtxt {
    template<class Combiner>
    class ShapeDistanceFinderImpl {
        public:
            explicit ShapeDistanceFinderImpl(const Shape& shape)
                : shape{shape}, combiner{shape}, edgeCache(shape.edgeCount()) {}

            Distance distance(const vec2& origin) {
                combiner.reset(origin);

                EdgeCache* cache = edgeCache.data();

                for (auto contourIt = shape.contours.begin(); contourIt != shape.contours.end(); ++contourIt) {
                    const Contour& contour = *contourIt;
                    if (contour.edges.empty()) {
                        continue;
                    }

                    DistanceSelector& selector = combiner.getSelector(int(contourIt - shape.contours.begin()));

                    const Segment* prevEdge = contour.edges.size() >= 2
                        ? (contour.edges.end() - 2)->get()
                        : contour.edges.begin()->get();
                    const Segment* curEdge = contour.edges.back().get();

                    for (const Edge& edge : contour.edges) {
                        const Segment* nextEdge = edge.get();
                        selector.add(*cache++, prevEdge, curEdge, nextEdge);
                        prevEdge = curEdge;
                        curEdge = nextEdge;
                    }
                }

                return combiner.distance();
            }

            static Distance oneShotDistance(const Shape& shape, const vec2& origin) {
                Combiner combiner(shape);
                combiner.reset(origin);

                for (auto contourIt = shape.contours.begin(); contourIt != shape.contours.end(); ++contourIt) {
                    const Contour& contour = *contourIt;
                    if (contour.edges.empty()) {
                        continue;
                    }

                    DistanceSelector& selector = combiner.getSelector(int(contourIt - shape.contours.begin()));

                    const Segment* prevEdge = contour.edges.size() >= 2
                        ? (contour.edges.end() - 2)->get()
                        : contour.edges.begin()->get();
                    const Segment* curEdge = contour.edges.back().get();

                    for (const Edge& edge : contour.edges) {
                        const Segment* nextEdge = edge.get();
                        EdgeCache dummy;
                        selector.add(dummy, prevEdge, curEdge, nextEdge);
                        prevEdge = curEdge;
                        curEdge = nextEdge;
                    }
                }

                return combiner.distance();
            }

        private:
            const Shape& shape;
            Combiner combiner;
            std::vector<EdgeCache> edgeCache;
    };

    // Public, non-template-parameterized finder. Picks the combiner at
    // construction time via overlapSupport instead of at compile time.
    class ShapeDistanceFinder {
        public:
            ShapeDistanceFinder(const Shape& shape, bool overlapSupport);

            Distance distance(const vec2& origin);

        private:
            bool overlapSupport;
            // Only one of these is engaged, chosen by overlapSupport.
            ShapeDistanceFinderImpl<ContourCombiner> simple;
            ShapeDistanceFinderImpl<OverlappingContourCombiner> overlapping;
    };
}

#endif