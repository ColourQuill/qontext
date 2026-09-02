#include <msdf/core/distance_selector.hpp>

// info
#include <info/properties.hpp>

namespace qtxt {
    bool GetPerpendicularDistance(double& distance, const vec2& ep, const vec2& edgeDir) {
        double ts = qbn::dot(ep, edgeDir);
        if (ts > 0) {
            double perpendicularDistance = qbn::cross(ep, edgeDir);
            if (fabs(perpendicularDistance) < fabs(distance)) {
                distance = perpendicularDistance;
                return true;
            }
        }
        return false;
    }

    void DistanceSelector::reset(const vec2& p) {
        double delta = DISTANCE_DELTA_FACTOR * qbn::length(p - this->p);
        r.reset(delta);
        g.reset(delta);
        b.reset(delta);
        this->p = p;
    }
    void DistanceSelector::add(EdgeCache& cache, const Segment* prevEdge, const Segment* edge, const Segment* nextEdge) {
        if ((hasRed(edge->color) && r.isRelevant(cache, p)) || (hasGreen(edge->color) && g.isRelevant(cache, p)) || (hasBlue(edge->color) && b.isRelevant(cache, p))) {
            double param;
            SignedDistance distance = edge->getSignedDistance(p, param);
            if (hasRed(edge->color))
                r.addTrueDistance(edge, distance, param);
            if (hasGreen(edge->color))
                g.addTrueDistance(edge, distance, param);
            if (hasBlue(edge->color))
                b.addTrueDistance(edge, distance, param);
            cache.point = p;
            cache.absDistance = fabs(distance.distance);

            vec2 ap = p - edge->point(0);
            vec2 bp = p - edge->point(1);
            vec2 aDir = qbn::norm(edge->direction(0), true);
            vec2 bDir = qbn::norm(edge->direction(1), true);
            vec2 prevDir = qbn::norm(prevEdge->direction(1), true);
            vec2 nextDir = qbn::norm(nextEdge->direction(0), true);
            double add = qbn::dot(ap, qbn::norm(prevDir + aDir, true));
            double bdd = -qbn::dot(bp, qbn::norm(bDir + nextDir, true));
            if (add > 0) {
                double pd = distance.distance;
                if (GetPerpendicularDistance(pd, ap, -aDir)) {
                    pd = -pd;
                    if (hasRed(edge->color))
                        r.addDistance(pd);
                    if (hasGreen(edge->color))
                        g.addDistance(pd);
                    if (hasBlue(edge->color))
                        b.addDistance(pd);
                }
                cache.aPerpendicularDistance = pd;
            }
            if (bdd > 0) {
                double pd = distance.distance;
                if (GetPerpendicularDistance(pd, bp, bDir)) {
                    if (hasRed(edge->color))
                        r.addDistance(pd);
                    if (hasGreen(edge->color))
                        g.addDistance(pd);
                    if (hasBlue(edge->color))
                        b.addDistance(pd);
                }
                cache.bPerpendicularDistance = pd;
            }
            cache.aDomainDistance = add;
            cache.bDomainDistance = bdd;
        }
    }
    void DistanceSelector::merge(const DistanceSelector& other) {
        r.merge(other.r);
        g.merge(other.g);
        b.merge(other.b);
    }
    Distance DistanceSelector::distance() const {
        Distance dist;
        dist.r = r.compute(p);
        dist.g = g.compute(p);
        dist.b = b.compute(p);
        return dist;
    }
    SignedDistance DistanceSelector::trueDistance() const {
        SignedDistance dist = r.minTrueDistance;
        if (g.minTrueDistance < dist) {
            dist = g.minTrueDistance;
        }
        if (b.minTrueDistance < dist) {
            dist = b.minTrueDistance;
        }
        return dist;
    }
}