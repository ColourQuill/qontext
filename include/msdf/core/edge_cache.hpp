#ifndef __QONTEXT_EDGE_CACHE_HPP__
#define __QONTEXT_EDGE_CACHE_HPP__

// msdf
#include <msdf/core/vec2.hpp>

namespace qtxt {
    struct EdgeCache {
        vec2 point;
        double absDistance;
        double aDomainDistance, bDomainDistance;
        double aPerpendicularDistance, bPerpendicularDistance;
        EdgeCache();
    };
}

#endif