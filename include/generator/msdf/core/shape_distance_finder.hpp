#ifndef __QONTEXT_SHAPE_DISTANCE_FINDER_HPP__
#define __QONTEXT_SHAPE_DISTANCE_FINDER_HPP__

// std
#include <vector>

// qarbon
#include <qarbon.hpp>

// msdf
#include <generator/msdf/core/edge_selectors.hpp>
#include <generator/msdf/core/contour_combiners.hpp>
#include <generator/msdf/core/shape.hpp>
#include <generator/msdf/core/contour.hpp>

namespace qtxt {
    template<class ContourCombiner>
    class ShapeDistanceFinder {
        public:
            typedef typename ContourCombiner::DistanceType DistanceType;

            explicit ShapeDistanceFinder(const Shape& shape) : shape{shape}, contourCombiner{shape}, shapeEdgeCache{std::vector<typename ContourCombiner::EdgeSelectorType::EdgeCache>(shape.EdgeCount())} {}
            DistanceType Distance(const vec2& origin) {
                contourCombiner.Reset(origin);

                typename ContourCombiner::EdgeSelectorType::EdgeCache* edgeCache = shapeEdgeCache.data();

                for (std::vector<Contour>::const_iterator contour = shape.contours.begin(); contour != shape.contours.end(); ++contour) {
                    if (!contour->edges.empty()) {
                        typename ContourCombiner::EdgeSelectorType& edgeSelector = contourCombiner.GetEdgeSelector(int(contour - shape.contours.begin()));
                        
                        const EdgeSegment* prevEdge = contour->edges.size() >= 2 ? *(contour->edges.end() - 2) : *contour->edges.begin();
                        const EdgeSegment* curEdge = contour->edges.back();
                        for (const EdgeHolder& edge : contour->edges) {
                            const EdgeSegment* nextEdge = edge;
                            edgeSelector.AddEdge(*edgeCache++, prevEdge, curEdge, nextEdge);
                            prevEdge = curEdge;
                            curEdge = nextEdge;
                        }
                    }
                }

                return contourCombiner.Distance();
            }

            static DistanceType OneShotDistance(const Shape& shape, const vec2& origin) {
                ContourCombiner contourCombiner{shape};
                contourCombiner.Reset(origin);

                for (std::vector<Contour>::const_iterator contour = shape.contours.begin(); contour != shape.contours.end(); ++contour) {
                    if (!contour->edges.empty()) {
                        typename ContourCombiner::EdgeSelectorType& edgeSelector = contourCombiner.GetEdgeSelector(int(contour - shape.contours.begin()));

                        const EdgeSegment* prevEdge = contour->edges.size() >= 2 ? *(contour->edges.end() - 2) : *contour->edges.begin();
                        const EdgeSegment* curEdge = contour->edges.back();
                        for (const EdgeHolder& edge : contour->edges) {
                            const EdgeSegment* nextEdge = edge;
                            typename ContourCombiner::EdgeSelectorType::EdgeCache dummy;
                            edgeSelector.AddEdge(dummy, prevEdge, curEdge, nextEdge);
                            prevEdge = curEdge;
                            curEdge = nextEdge;
                        }
                    }
                }

                return contourCombiner.Distance();
            }
        private:
            const Shape& shape;
            ContourCombiner contourCombiner;
            std::vector<typename ContourCombiner::EdgeSelectorType::EdgeCache> shapeEdgeCache;
    };

    typedef ShapeDistanceFinder<SimpleContourCombiner<TrueDistanceSelector>> SimpleTrueShapeDistanceFinder;
}

#endif