#include <msdf/core/contour_combiners.hpp>

// msdf
#include <msdf/core/arithmetic.hpp>
#include <msdf/core/shape/shape.hpp>
#include <msdf/core/shape/contour.hpp>

namespace qtxt {
    static double resolveDistance(const Distance& d) {
        return median(d.r, d.g, d.b);
    }

    ContourCombiner::ContourCombiner(const Shape&) {}
    void ContourCombiner::reset(const vec2& p) {
        selector.reset(p);
    }
    DistanceSelector& ContourCombiner::getSelector(int) {
        return selector;
    }
    Distance ContourCombiner::distance() const {
        return selector.distance();
    }

    OverlappingContourCombiner::OverlappingContourCombiner(const Shape& shape) {
        windings.reserve(shape.contours.size());
        for (const Contour& contour : shape.contours) {
            windings.push_back(contour.winding());
        }
        selectors.resize(shape.contours.size());
    }
    void OverlappingContourCombiner::reset(const vec2& p) {
        this->p = p;
        for (auto& s : selectors) {
            s.reset(p);
        }
    }
    DistanceSelector& OverlappingContourCombiner::getSelector(int i) {
        return selectors[i];
    }
    Distance OverlappingContourCombiner::distance() const {
        int n = (int)selectors.size();
        DistanceSelector shapeSel, innerSel, outerSel;
        shapeSel.reset(p); innerSel.reset(p); outerSel.reset(p);
        for (int i = 0; i < n; ++i) {
            Distance ed = selectors[i].distance();
            shapeSel.merge(selectors[i]);
            if (windings[i] > 0 && resolveDistance(ed) >= 0) innerSel.merge(selectors[i]);
            if (windings[i] < 0 && resolveDistance(ed) <= 0) outerSel.merge(selectors[i]);
        }
        Distance shapeDist = shapeSel.distance();
        Distance innerDist = innerSel.distance();
        Distance outerDist = outerSel.distance();
        double innerScalar = resolveDistance(innerDist);
        double outerScalar = resolveDistance(outerDist);
        Distance distance{-DBL_MAX, -DBL_MAX, -DBL_MAX};
        int winding = 0;

        if (innerScalar >= 0 && fabs(innerScalar) <= fabs(outerScalar)) {
            distance = innerDist; winding = 1;
            for (int i = 0; i < n; ++i) if (windings[i] > 0) {
                Distance cd = selectors[i].distance();
                if (fabs(resolveDistance(cd)) < fabs(outerScalar) && resolveDistance(cd) > resolveDistance(distance)) distance = cd;
            }
        } else if (outerScalar <= 0 && fabs(outerScalar) < fabs(innerScalar)) {
            distance = outerDist; winding = -1;
            for (int i = 0; i < n; ++i) if (windings[i] < 0) {
                Distance cd = selectors[i].distance();
                if (fabs(resolveDistance(cd)) < fabs(innerScalar) && resolveDistance(cd) < resolveDistance(distance)) distance = cd;
            }
        } else return shapeDist;

        for (int i = 0; i < n; ++i) if (windings[i] != winding) {
            Distance cd = selectors[i].distance();
            if (resolveDistance(cd) * resolveDistance(distance) >= 0 && fabs(resolveDistance(cd)) < fabs(resolveDistance(distance))) distance = cd;
        }
        if (resolveDistance(distance) == resolveDistance(shapeDist)) distance = shapeDist;
        return distance;
    }
}