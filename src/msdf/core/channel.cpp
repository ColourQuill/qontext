#include <msdf/core/channel.hpp>

// info
#include <info/properties.hpp>

// msdf
#include <msdf/core/arithmetic.hpp>

namespace qtxt {
    void Channel::reset(double delta) {
        minTrueDistance.distance += nonZeroSign(minTrueDistance.distance) * delta;
        minNegativePerpendicularDistance = -fabs(minTrueDistance.distance);
        minPositivePerpendicularDistance = fabs(minTrueDistance.distance);
        nearEdge = NULL;
        nearEdgeParam = 0;
    }
    void Channel::addTrueDistance(const Segment* edge, const SignedDistance& distance, double param) {
        if (distance < minTrueDistance) {
            minTrueDistance = distance;
            nearEdge = edge;
            nearEdgeParam = param;
        }
    }
    void Channel::addDistance(double distance) {
        if (distance <= 0 && distance > minNegativePerpendicularDistance) {
            minNegativePerpendicularDistance = distance;
        }
        if (distance >= 0 && distance < minPositivePerpendicularDistance) {
            minPositivePerpendicularDistance = distance;
        }
    }
    void Channel::merge(const Channel& other) {
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
    double Channel::compute(const vec2& p) const {
        double minDistance = minTrueDistance.distance < 0 ? minNegativePerpendicularDistance : minPositivePerpendicularDistance;
        if (nearEdge) {
            SignedDistance distance = minTrueDistance;
            nearEdge->distanceToPerpendicularDistance(distance, p, nearEdgeParam);
            if (fabs(distance.distance) < fabs(minDistance)) {
                minDistance = distance.distance;
            }
        }
        return minDistance;
    }
    bool Channel::isRelevant(const EdgeCache& cache, const vec2& p) const {
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
}