#ifndef __QONTEXT_ATLAS_CONFIG_HPP__
#define __QONTEXT_ATLAS_CONFIG_HPP__

// msdf
#include <msdf/atlas/types.hpp>

namespace qtxt {
    struct AtlasConfig {
        DimensionsConstraint constraint = DimensionsConstraint::SQUARE;
        double minScale = 24.0;
        double pxRange = 2.0;
        double maxCornerAngle = 3.0;
        int threadCount = 4;
        bool alignPxOrigin = false;
        int spacing = 0;
    };
}

#endif