#ifndef __QONTEXT_MSDF_GENERATOR_CONFIG_HPP__
#define __QONTEXT_MSDF_GENERATOR_CONFIG_HPP__

// std
#include <cstdint>

// msdf
#include <generator/msdf/core/image_ref.hpp>

namespace qtxt {
    struct ErrorCorrectionConfig {
        static const double defaultMinDeviationRatio;
        static const double defaultMinImproveRatio;

        enum Mode {
            DISABLED,
            INDISCRIMINATE,
            EDGE_PRIORITY,
            EDGE_ONLY
        } mode;

        enum DistanceCheckMode {
            DO_NOT_CHECK_DISTANCE,
            CHECK_DISTANCE_AT_EDGE,
            ALWAYS_CHECK_DISTANCE
        } distanceCheckMode;

        double minDeviationRatio;
        double minImproveRatio;
        uint8_t* buffer;

        inline explicit ErrorCorrectionConfig(Mode mode = EDGE_PRIORITY, DistanceCheckMode distanceCheckMode = CHECK_DISTANCE_AT_EDGE, double minDeviationRatio = defaultMinDeviationRatio, double minImproveRatio = defaultMinImproveRatio, uint8_t* buffer = nullptr) : mode{mode}, distanceCheckMode{distanceCheckMode}, minDeviationRatio{minDeviationRatio}, minImproveRatio{minImproveRatio}, buffer{buffer} {}
    };

    struct GeneratorConfig {
        bool overlapSupport;
        inline explicit GeneratorConfig(bool overlapSupport = true) : overlapSupport{overlapSupport} {}
    };

    struct MSDFGeneratorConfig : GeneratorConfig {
        ErrorCorrectionConfig errorCorrection;

        inline MSDFGeneratorConfig() {}
        inline explicit MSDFGeneratorConfig(bool overlapSupport, const ErrorCorrectionConfig& errorCorrection = ErrorCorrectionConfig()) : GeneratorConfig{overlapSupport}, errorCorrection{errorCorrection} {}
    };
}

#endif