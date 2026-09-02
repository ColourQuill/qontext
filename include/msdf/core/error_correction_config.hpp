#ifndef __QONTEXT_ERROR_CORRECTION_CONFIG_HPP__
#define __QONTEXT_ERROR_CORRECTION_CONFIG_HPP__

// std
#include <cstdint>

// info
#include <info/properties.hpp>

namespace qtxt {
    struct ErrorCorrectionConfig {
        enum Mode {
            DISABLED,
            INDISCRIMINATE,
            EDGE_PRIORITY,
            EDGE_ONLY
        };
        enum DistanceCheckMode {
            DO_NOT_CHECK_DISTANCE,
            CHECK_DISTANCE_AT_EDGE,
            ALWAYS_CHECK_DISTANCE
        };

        Mode mode = EDGE_PRIORITY;
        DistanceCheckMode distanceCheckMode = CHECK_DISTANCE_AT_EDGE;
        double minDeviationRatio = DEFAULT_MIN_DEVIATION_RATIO;
        double minImproveRatio = DEFAULT_MIN_IMPROVE_RATIO;
        uint8_t* buffer = nullptr;
    };
}

#endif