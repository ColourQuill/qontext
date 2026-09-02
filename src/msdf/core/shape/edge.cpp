#include <msdf/core/shape/edge.hpp>

namespace qtxt {
    Edge::~Edge() {
        if (segment) delete segment;
    }
    Edge::Edge(const Edge& other) : segment(other.segment ? other.segment->clone() : nullptr) {
    }
    Edge::Edge(Edge&& other) noexcept : segment(other.segment) {
        other.segment = nullptr;
    }
    Edge& Edge::operator=(const Edge& other) {
        if (this != &other) {
            delete segment;
            segment = other.segment ? other.segment->clone() : nullptr;
        }
        return *this;
    }
    Edge& Edge::operator=(Edge&& other) noexcept {
        if (this != &other) {
            delete segment;
            segment = other.segment;
            other.segment = nullptr;
        }
        return *this;
    }
    void Edge::swap(Edge& a, Edge& b) {
        Segment* tmp = a.segment;
        a.segment = b.segment;
        b.segment = tmp;
    }
    Segment* Edge::get() const {
        return segment;
    }
    bool Edge::empty() const {
        return segment == nullptr;
    }
}