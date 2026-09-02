#ifndef __QONTEXT_DISTANCE_SELECTOR_HPP__
#define __QONTEXT_DISTANCE_SELECTOR_HPP__

// msdf
#include <msdf/core/distance.hpp>
#include <msdf/core/edge_cache.hpp>
#include <msdf/core/channel.hpp>

namespace qtxt {
    class DistanceSelector {
        public:
            void reset(const vec2& p);
            void add(EdgeCache& cache, const Segment* prevEdge, const Segment* edge, const Segment* nextEdge);
            void merge(const DistanceSelector& other);
            Distance distance() const;
            SignedDistance trueDistance() const;
        private:
            Channel r, g, b;
            vec2 p;
    };
}

#endif