#ifndef __QONTEXT_EDGE_SELECTORS_HPP__
#define __QONTEXT_EDGE_SELECTORS_HPP__

// qarbon
#include <qarbon.hpp>

// msdf
#include <generator/msdf/core/signed_distance.hpp>

namespace qtxt {
    using vec2 = qbn::vec2<double>;

    // forward declarations
    class EdgeSegment;

    struct MultiDistance {
        double r, g, b;
    };
    struct MultiAndTrueDistance : MultiDistance {
        double a;
    };

    class TrueDistanceSelector {
        public:
            typedef double DistanceType;

            struct EdgeCache {
                vec2 point;
                double absDistance;

                EdgeCache();
            };

            void Reset(const vec2& p);
            void AddEdge(EdgeCache& cache, const EdgeSegment* prevEdge, const EdgeSegment* edge, const EdgeSegment* nextEdge);
            void Merge(const TrueDistanceSelector& other);
            DistanceType Distance() const;
        private:
            vec2 p;
            SignedDistance minDistance;
    };

    class PerpendicularDistanceSelectorBase {
        public:
            struct EdgeCache {
                vec2 point;
                double absDistance;
                double aDomainDistance, bDomainDistance;
                double aPerpendicularDistance, bPerpendicularDistance;

                EdgeCache();
            };

            static bool GetPerpendicularDistance(double& distance, const vec2& ep, const vec2& edgeDir);

            PerpendicularDistanceSelectorBase();
            void Reset(double delta);
            bool IsEdgeRelevant(const EdgeCache& cache, const EdgeSegment* edge, const vec2& p) const;
            void AddEdgeTrueDistance(const EdgeSegment* edge, const SignedDistance& distance, double param);
            void AddEdgePerpendicularDistance(double distance);
            void Merge(const PerpendicularDistanceSelectorBase& other);
            double ComputeDistance(const vec2& p) const;
            SignedDistance TrueDistance() const;
        private:
            SignedDistance minTrueDistance;
            double minNegativePerpendicularDistance;
            double minPositivePerpendicularDistance;
            const EdgeSegment* nearEdge;
            double nearEdgeParam;
    };

    class PerpendicularDistanceSelector : public PerpendicularDistanceSelectorBase {
        public:
            typedef double DistanceType;

            void Reset(const vec2& p);
            void AddEdge(EdgeCache& cache, const EdgeSegment* prevEdge, const EdgeSegment* edge, const EdgeSegment* nextEdge);
            DistanceType Distance() const;
        private:
            vec2 p;
    };

    class MultiDistanceSelector {
        public:
            typedef MultiDistance DistanceType;
            typedef PerpendicularDistanceSelectorBase::EdgeCache EdgeCache;

            void Reset(const vec2& p);
            void AddEdge(EdgeCache& cache, const EdgeSegment* prevEdge, const EdgeSegment* edge, const EdgeSegment* nextEdge);
            void Merge(const MultiDistanceSelector& other);
            DistanceType Distance() const;
            SignedDistance TrueDistance() const;
        private:
            vec2 p;
            PerpendicularDistanceSelectorBase r, g, b;
    };

    class MultiAndTrueDistanceSelector : public MultiDistanceSelector {
        public:
            typedef MultiAndTrueDistance DistanceType;

            DistanceType Distance() const;
    };
}

#endif