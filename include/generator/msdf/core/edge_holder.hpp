#ifndef __QONTEXT_EDGE_HOLDER_HPP__
#define __QONTEXT_EDGE_HOLDER_HPP__

// qarbon
#include <qarbon.hpp>

// msdf
#include <generator/msdf/core/edge_color.hpp>
#include <generator/msdf/core/edge_segments.hpp>

using vec2 = qbn::vec2<double>;

namespace qtxt {
    class EdgeHolder {
        public:
            static void Swap(EdgeHolder& a, EdgeHolder& b);

            inline EdgeHolder() : pEdgeSegment{} {}
            inline EdgeHolder(EdgeSegment* pEdgeSegment) : pEdgeSegment{pEdgeSegment} {}
            inline EdgeHolder(vec2 p0, vec2 p1, EdgeColor edgeColor = WHITE) : pEdgeSegment{EdgeSegment::Create(p0, p1, edgeColor)} {}
            inline EdgeHolder(vec2 p0, vec2 p1, vec2 p2, EdgeColor edgeColor = WHITE) : pEdgeSegment{EdgeSegment::Create(p0, p1, p2, edgeColor)} {}
            inline EdgeHolder(vec2 p0, vec2 p1, vec2 p2, vec2 p3, EdgeColor edgeColor = WHITE) : pEdgeSegment{EdgeSegment::Create(p0, p1, p2, p3, edgeColor)} {}
            EdgeHolder(const EdgeHolder& orig);
            ~EdgeHolder();
            EdgeHolder& operator=(const EdgeHolder& orig);
            EdgeSegment& operator*();
            const EdgeSegment& operator*() const;
            EdgeSegment* operator->();
            const EdgeSegment* operator->() const;
            operator EdgeSegment*();
            operator const EdgeSegment*() const;
        private:
            EdgeSegment* pEdgeSegment;
    };
}

#endif