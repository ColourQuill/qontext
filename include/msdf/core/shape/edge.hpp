#ifndef __QONTEXT_EDGE_HPP__
#define __QONTEXT_EDGE_HPP__

// msdf
#include <msdf/core/vec2.hpp>
#include <msdf/core/shape/segment.hpp>

namespace qtxt {
    class Edge {
        public:
            static void swap(Edge& a, Edge& b);
            ~Edge();
            inline Edge() : segment{} {}
            inline Edge(Segment* segment) : segment{segment} {}
            inline Edge(vec2 p0, vec2 p1, Color color = Color::WHITE) : segment{Segment::create(p0, p1, color)} {}
            inline Edge(vec2 p0, vec2 p1, vec2 p2, Color color = Color::WHITE) : segment{Segment::create(p0, p1, p2, color)} {}
            inline Edge(vec2 p0, vec2 p1, vec2 p2, vec2 p3, Color color = Color::WHITE) : segment{Segment::create(p0, p1, p2, p3, color)} {}

            Edge(const Edge& other);
            Edge(Edge&& other) noexcept;
            Edge& operator=(const Edge& other);
            Edge& operator=(Edge&& other) noexcept;

            Segment* get() const;
            bool empty() const;
        private:
            Segment* segment = nullptr;
    };
}

#endif