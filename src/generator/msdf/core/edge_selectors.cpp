#include <generator/msdf/core/edge_selectors.hpp>

#define DISTANCE_DELTA_FACTOR 1.001

// msdf
#include <generator/msdf/core/arithmetic.hpp>
#include <generator/msdf/core/edge_segments.hpp>

#include <iostream>

namespace qtxt {
    TrueDistanceSelector::EdgeCache::EdgeCache() : absDistance{0} {}

    void TrueDistanceSelector::Reset(const vec2& p) {
        double delta = DISTANCE_DELTA_FACTOR * qbn::length(p - this->p);
        minDistance.distance += nonZeroSign(minDistance.distance) * delta;
        this->p = p;
    }
    void TrueDistanceSelector::AddEdge(EdgeCache& cache, const EdgeSegment* prevEdge, const EdgeSegment* edge, const EdgeSegment* nextEdge) {
        double delta = DISTANCE_DELTA_FACTOR * qbn::length(p - cache.point);
        if (cache.absDistance-delta <= fabs(minDistance.distance)) {
            double dummy;
            SignedDistance distance = edge->GetSignedDistance(p, dummy);
            if (distance < minDistance) {
                minDistance = distance;
            }
            cache.point = p;
            cache.absDistance = fabs(distance.distance);
        }
    }
    void TrueDistanceSelector::Merge(const TrueDistanceSelector& other) {
        if (other.minDistance < minDistance) {
            minDistance = other.minDistance;
        }
    }
    TrueDistanceSelector::DistanceType TrueDistanceSelector::Distance() const {
        return minDistance.distance;
    }

    PerpendicularDistanceSelectorBase::EdgeCache::EdgeCache() : absDistance{0}, aDomainDistance{0}, bDomainDistance{0}, aPerpendicularDistance{0}, bPerpendicularDistance{0} {}

    bool PerpendicularDistanceSelectorBase::GetPerpendicularDistance(double &distance, const vec2& ep, const vec2& edgeDir) {
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

    PerpendicularDistanceSelectorBase::PerpendicularDistanceSelectorBase() : minNegativePerpendicularDistance(-fabs(minTrueDistance.distance)), minPositivePerpendicularDistance(fabs(minTrueDistance.distance)), nearEdge(NULL), nearEdgeParam(0) { }

    void PerpendicularDistanceSelectorBase::Reset(double delta) {
        minTrueDistance.distance += nonZeroSign(minTrueDistance.distance) * delta;
        minNegativePerpendicularDistance = -fabs(minTrueDistance.distance);
        minPositivePerpendicularDistance = fabs(minTrueDistance.distance);
        nearEdge = NULL;
        nearEdgeParam = 0;
    }

    bool PerpendicularDistanceSelectorBase::IsEdgeRelevant(const EdgeCache& cache, const EdgeSegment* edge, const vec2& p) const {
        double delta = DISTANCE_DELTA_FACTOR * qbn::length(p - cache.point);
        return (
            cache.absDistance - delta <= fabs(minTrueDistance.distance) ||
            fabs(cache.aDomainDistance) < delta ||
            fabs(cache.bDomainDistance) < delta ||
            (cache.aDomainDistance > 0 && (cache.aPerpendicularDistance < 0 ?
                cache.aPerpendicularDistance + delta >= minNegativePerpendicularDistance :
                cache.aPerpendicularDistance - delta <= minPositivePerpendicularDistance
            )) ||
            (cache.bDomainDistance > 0 && (cache.bPerpendicularDistance < 0 ?
                cache.bPerpendicularDistance + delta >= minNegativePerpendicularDistance :
                cache.bPerpendicularDistance - delta <= minPositivePerpendicularDistance
            ))
        );
    }

    void PerpendicularDistanceSelectorBase::AddEdgeTrueDistance(const EdgeSegment* edge, const SignedDistance& distance, double param) {
        if (distance < minTrueDistance) {
            minTrueDistance = distance;
            nearEdge = edge;
            nearEdgeParam = param;
        }
    }

    void PerpendicularDistanceSelectorBase::AddEdgePerpendicularDistance(double distance) {
        if (distance <= 0 && distance > minNegativePerpendicularDistance) {
            minNegativePerpendicularDistance = distance;
        }
        if (distance >= 0 && distance < minPositivePerpendicularDistance) {
            minPositivePerpendicularDistance = distance;
        }
    }

    void PerpendicularDistanceSelectorBase::Merge(const PerpendicularDistanceSelectorBase& other) {
        if (other.minTrueDistance < minTrueDistance) {
            minTrueDistance = other.minTrueDistance;
            nearEdge = other.nearEdge;
            nearEdgeParam = other.nearEdgeParam;
        }
        if (other.minNegativePerpendicularDistance > minNegativePerpendicularDistance) {
            minNegativePerpendicularDistance = other.minNegativePerpendicularDistance;
        }
        if (other.minPositivePerpendicularDistance < minPositivePerpendicularDistance) {
            minPositivePerpendicularDistance = other.minPositivePerpendicularDistance;
        }
    }

    double PerpendicularDistanceSelectorBase::ComputeDistance(const vec2& p) const {
        double minDistance = minTrueDistance.distance < 0 ? minNegativePerpendicularDistance : minPositivePerpendicularDistance;
        if (nearEdge) {
            SignedDistance distance = minTrueDistance;
            nearEdge->DistanceToPerpendicularDistance(distance, p, nearEdgeParam);
            if (fabs(distance.distance) < fabs(minDistance)) {
                minDistance = distance.distance;
            }
        }
        return minDistance;
    }

    SignedDistance PerpendicularDistanceSelectorBase::TrueDistance() const {
        return minTrueDistance;
    }

    void PerpendicularDistanceSelector::Reset(const vec2& p) {
        double delta = DISTANCE_DELTA_FACTOR * qbn::length(p - this->p);
        PerpendicularDistanceSelectorBase::Reset(delta);
        this->p = p;
    }

    void PerpendicularDistanceSelector::AddEdge(EdgeCache& cache, const EdgeSegment* prevEdge, const EdgeSegment* edge, const EdgeSegment* nextEdge) {
        if (IsEdgeRelevant(cache, edge, p)) {
            double param;
            SignedDistance distance = edge->GetSignedDistance(p, param);
            AddEdgeTrueDistance(edge, distance, param);
            cache.point = p;
            cache.absDistance = fabs(distance.distance);

            vec2 ap = p-edge->Point(0);
            vec2 bp = p-edge->Point(1);
            vec2 aDir = qbn::norm(edge->Direction(0), true);
            vec2 bDir = qbn::norm(edge->Direction(1), true);
            vec2 prevDir = qbn::norm(prevEdge->Direction(1), true);
            vec2 nextDir = qbn::norm(nextEdge->Direction(0), true);
            double add = qbn::dot(ap, qbn::norm(prevDir + aDir, true));
            double bdd = -qbn::dot(bp, qbn::norm(bDir + nextDir, true));
            if (add > 0) {
                double pd = distance.distance;
                if (GetPerpendicularDistance(pd, ap, -aDir)) {
                    AddEdgePerpendicularDistance(pd = -pd);
                }
                cache.aPerpendicularDistance = pd;
            }
            if (bdd > 0) {
                double pd = distance.distance;
                if (GetPerpendicularDistance(pd, bp, bDir)) {
                    AddEdgePerpendicularDistance(pd);
                }
                cache.bPerpendicularDistance = pd;
            }
            cache.aDomainDistance = add;
            cache.bDomainDistance = bdd;
        }
    }

    PerpendicularDistanceSelector::DistanceType PerpendicularDistanceSelector::Distance() const {
        return ComputeDistance(p);
    }

    void MultiDistanceSelector::Reset(const vec2& p) {
        double delta = DISTANCE_DELTA_FACTOR * qbn::length(p - this->p);
        r.Reset(delta);
        g.Reset(delta);
        b.Reset(delta);
        this->p = p;
    }

    void MultiDistanceSelector::AddEdge(EdgeCache& cache, const EdgeSegment* prevEdge, const EdgeSegment* edge, const EdgeSegment* nextEdge) {
        if (
            (edge->color & RED && r.IsEdgeRelevant(cache, edge, p)) ||
            (edge->color & GREEN && g.IsEdgeRelevant(cache, edge, p)) ||
            (edge->color & BLUE && b.IsEdgeRelevant(cache, edge, p))
        ) {
            double param;
            SignedDistance distance = edge->GetSignedDistance(p, param);
            if (edge->color&RED)
                r.AddEdgeTrueDistance(edge, distance, param);
            if (edge->color&GREEN)
                g.AddEdgeTrueDistance(edge, distance, param);
            if (edge->color&BLUE)
                b.AddEdgeTrueDistance(edge, distance, param);
            cache.point = p;
            cache.absDistance = fabs(distance.distance);

            vec2 ap = p-edge->Point(0);
            vec2 bp = p-edge->Point(1);
            vec2 aDir = qbn::norm(edge->Direction(0), true);
            vec2 bDir = qbn::norm(edge->Direction(1), true);
            vec2 prevDir = qbn::norm(prevEdge->Direction(1), true);
            vec2 nextDir = qbn::norm(nextEdge->Direction(0), true);
            double add = qbn::dot(ap, qbn::norm(prevDir + aDir, true));
            double bdd = -qbn::dot(bp, qbn::norm(bDir + nextDir, true));
            if (add > 0) {
                double pd = distance.distance;
                if (PerpendicularDistanceSelectorBase::GetPerpendicularDistance(pd, ap, -aDir)) {
                    pd = -pd;
                    if (edge->color & RED)
                        r.AddEdgePerpendicularDistance(pd);
                    if (edge->color & GREEN)
                        g.AddEdgePerpendicularDistance(pd);
                    if (edge->color & BLUE)
                        b.AddEdgePerpendicularDistance(pd);
                }
                cache.aPerpendicularDistance = pd;
            }
            if (bdd > 0) {
                double pd = distance.distance;
                if (PerpendicularDistanceSelectorBase::GetPerpendicularDistance(pd, bp, bDir)) {
                    if (edge->color & RED)
                        r.AddEdgePerpendicularDistance(pd);
                    if (edge->color & GREEN)
                        g.AddEdgePerpendicularDistance(pd);
                    if (edge->color & BLUE)
                        b.AddEdgePerpendicularDistance(pd);
                }
                cache.bPerpendicularDistance = pd;
            }
            cache.aDomainDistance = add;
            cache.bDomainDistance = bdd;
        }
    }

    void MultiDistanceSelector::Merge(const MultiDistanceSelector& other) {
        r.Merge(other.r);
        g.Merge(other.g);
        b.Merge(other.b);
    }

    MultiDistanceSelector::DistanceType MultiDistanceSelector::Distance() const {
        MultiDistance multiDistance;
        multiDistance.r = r.ComputeDistance(p);
        multiDistance.g = g.ComputeDistance(p);
        multiDistance.b = b.ComputeDistance(p);
        return multiDistance;
    }

    SignedDistance MultiDistanceSelector::TrueDistance() const {
        SignedDistance distance = r.TrueDistance();
        if (g.TrueDistance() < distance) {
            distance = g.TrueDistance();
        }
        if (b.TrueDistance() < distance) {
            distance = b.TrueDistance();
        }
        return distance;
    }

    MultiAndTrueDistanceSelector::DistanceType MultiAndTrueDistanceSelector::Distance() const {
        MultiDistance multiDistance = MultiDistanceSelector::Distance();
        MultiAndTrueDistance mtd;
        mtd.r = multiDistance.r;
        mtd.g = multiDistance.g;
        mtd.b = multiDistance.b;
        mtd.a = TrueDistance().distance;
        return mtd;
    }
}