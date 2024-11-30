#include <generator/msdf/core/contour_combiners.hpp>

// std
#include <cfloat>

// msdf
#include <generator/msdf/core/arithmetic.hpp>
#include <generator/msdf/core/shape.hpp>
#include <generator/msdf/core/contour.hpp>

namespace qtxt {
    static void InitDistance(double& distance) {
        distance = -DBL_MAX;
    }

    static void InitDistance(MultiDistance& distance) {
        distance.r = -DBL_MAX;
        distance.g = -DBL_MAX;
        distance.b = -DBL_MAX;
    }

    static void InitDistance(MultiAndTrueDistance& distance) {
        distance.r = -DBL_MAX;
        distance.g = -DBL_MAX;
        distance.b = -DBL_MAX;
        distance.a = -DBL_MAX;
    }

    static double ResolveDistance(double distance) {
        return distance;
    }

    static double ResolveDistance(const MultiDistance& distance) {
        return median(distance.r, distance.g, distance.b);
    }

    template<class EdgeSelector>
    SimpleContourCombiner<EdgeSelector>::SimpleContourCombiner(const Shape& shape) { }

    template<class EdgeSelector>
    void SimpleContourCombiner<EdgeSelector>::Reset(const vec2& p) {
        shapeEdgeSelector.Reset(p);
    }

    template<class EdgeSelector>
    EdgeSelector& SimpleContourCombiner<EdgeSelector>::GetEdgeSelector(int) {
        return shapeEdgeSelector;
    }

    template<class EdgeSelector>
    typename SimpleContourCombiner<EdgeSelector>::DistanceType SimpleContourCombiner<EdgeSelector>::Distance() const {
        return shapeEdgeSelector.Distance();
    }

    template class SimpleContourCombiner<TrueDistanceSelector>;
    template class SimpleContourCombiner<PerpendicularDistanceSelector>;
    template class SimpleContourCombiner<MultiDistanceSelector>;
    template class SimpleContourCombiner<MultiAndTrueDistanceSelector>;

    template<class EdgeSelector>
    OverlappingContourCombiner<EdgeSelector>::OverlappingContourCombiner(const Shape& shape) {
        windings.reserve(shape.contours.size());
        for (const Contour& contour : shape.contours) {
            windings.push_back(contour.Winding());
        }
        edgeSelectors.resize(shape.contours.size());
    }

    template<class EdgeSelector>
    void OverlappingContourCombiner<EdgeSelector>::Reset(const vec2& p) {
        this->p = p;
        for (typename std::vector<EdgeSelector>::iterator contourEdgeSelector = edgeSelectors.begin(); contourEdgeSelector != edgeSelectors.end(); ++contourEdgeSelector) {
            contourEdgeSelector->Reset(p);
        }
    }

    template<class EdgeSelector>
    EdgeSelector& OverlappingContourCombiner<EdgeSelector>::GetEdgeSelector(int i) {
        return edgeSelectors[i];
    }

    template<class EdgeSelector>
    typename OverlappingContourCombiner<EdgeSelector>::DistanceType OverlappingContourCombiner<EdgeSelector>::Distance() const {
        int contourCount = (int)edgeSelectors.size();
        EdgeSelector shapeEdgeSelector;
        EdgeSelector innerEdgeSelector;
        EdgeSelector outerEdgeSelector;
        shapeEdgeSelector.Reset(p);
        innerEdgeSelector.Reset(p);
        outerEdgeSelector.Reset(p);
        for (int i = 0; i < contourCount; ++i) {
            DistanceType edgeDistance = edgeSelectors[i].Distance();
            shapeEdgeSelector.Merge(edgeSelectors[i]);
            if (windings[i] > 0 && ResolveDistance(edgeDistance) >= 0) {
                innerEdgeSelector.Merge(edgeSelectors[i]);
            }
            if (windings[i] < 0 && ResolveDistance(edgeDistance) <= 0) {
                outerEdgeSelector.Merge(edgeSelectors[i]);
            }
        }

        DistanceType shapeDistance = shapeEdgeSelector.Distance();
        DistanceType innerDistance = innerEdgeSelector.Distance();
        DistanceType outerDistance = outerEdgeSelector.Distance();
        double innerScalarDistance = ResolveDistance(innerDistance);
        double outerScalarDistance = ResolveDistance(outerDistance);
        DistanceType distance;
        InitDistance(distance);

        int winding = 0;
        if (innerScalarDistance >= 0 && fabs(innerScalarDistance) <= fabs(outerScalarDistance)) {
            distance = innerDistance;
            winding = 1;
            for (int i = 0; i < contourCount; ++i)
                if (windings[i] > 0) {
                    DistanceType contourDistance = edgeSelectors[i].Distance();
                    if (fabs(ResolveDistance(contourDistance)) < fabs(outerScalarDistance) && ResolveDistance(contourDistance) > ResolveDistance(distance)) {
                        distance = contourDistance;
                    }
                }
        } else if (outerScalarDistance <= 0 && fabs(outerScalarDistance) < fabs(innerScalarDistance)) {
            distance = outerDistance;
            winding = -1;
            for (int i = 0; i < contourCount; ++i)
                if (windings[i] < 0) {
                    DistanceType contourDistance = edgeSelectors[i].Distance();
                    if (fabs(ResolveDistance(contourDistance)) < fabs(innerScalarDistance) && ResolveDistance(contourDistance) < ResolveDistance(distance)) {
                        distance = contourDistance;
                    }
                }
        } else
            return shapeDistance;

        for (int i = 0; i < contourCount; ++i)
            if (windings[i] != winding) {
                DistanceType contourDistance = edgeSelectors[i].Distance();
                if (ResolveDistance(contourDistance) * ResolveDistance(distance) >= 0 && fabs(ResolveDistance(contourDistance)) < fabs(ResolveDistance(distance))) {
                    distance = contourDistance;
                } 
            }
        if (ResolveDistance(distance) == ResolveDistance(shapeDistance)) {
            distance = shapeDistance;
        }
        return distance;
    }

    template class OverlappingContourCombiner<TrueDistanceSelector>;
    template class OverlappingContourCombiner<PerpendicularDistanceSelector>;
    template class OverlappingContourCombiner<MultiDistanceSelector>;
    template class OverlappingContourCombiner<MultiAndTrueDistanceSelector>;
}