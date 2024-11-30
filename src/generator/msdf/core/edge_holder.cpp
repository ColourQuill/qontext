#include <generator/msdf/core/edge_holder.hpp>

namespace qtxt {
    void EdgeHolder::Swap(EdgeHolder& a, EdgeHolder& b) {
        EdgeSegment* tmp = a.pEdgeSegment;
        a.pEdgeSegment = b.pEdgeSegment;
        b.pEdgeSegment = tmp;
    }

    EdgeHolder::EdgeHolder(const EdgeHolder& orig) : pEdgeSegment{orig.pEdgeSegment ? orig.pEdgeSegment->Clone() : NULL} {}
    EdgeHolder::~EdgeHolder() {
        delete pEdgeSegment;
    }
    EdgeHolder& EdgeHolder::operator=(const EdgeHolder& orig) {
        if (this != &orig) {
            delete pEdgeSegment;
            pEdgeSegment = orig.pEdgeSegment ? orig.pEdgeSegment->Clone() : NULL;
        }
        return *this;
    }
    EdgeSegment& EdgeHolder::operator*() {
        return *pEdgeSegment;
    }
    const EdgeSegment& EdgeHolder::operator*() const {
        return *pEdgeSegment;
    }
    EdgeSegment* EdgeHolder::operator->() {
        return pEdgeSegment;
    }
    const EdgeSegment* EdgeHolder::operator->() const {
        return pEdgeSegment;
    }
    EdgeHolder::operator EdgeSegment*() {
        return pEdgeSegment;
    }
    EdgeHolder::operator const EdgeSegment*() const {
        return pEdgeSegment;
    }
}