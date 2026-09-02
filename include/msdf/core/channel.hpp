#ifndef __QONTEXT_CHANNEL_HPP__
#define __QONTEXT_CHANNEL_HPP__

// msdf
#include <msdf/core/shape/signed_distance.hpp>
#include <msdf/core/shape/segment.hpp>
#include <msdf/core/edge_cache.hpp>

namespace qtxt {
    struct Channel {
        SignedDistance minTrueDistance;
        double minNegativePerpendicularDistance = -fabs(minTrueDistance.distance);
        double minPositivePerpendicularDistance = fabs(minTrueDistance.distance);
        const Segment* nearEdge = nullptr;
        double nearEdgeParam = 0;

        void reset(double delta);
        void addTrueDistance(const Segment* edge, const SignedDistance& distance, double param);
        void addDistance(double distance);
        void merge(const Channel& other);
        double compute(const vec2& p) const;
        bool isRelevant(const EdgeCache& cache, const vec2& p) const;
    };
}

#endif