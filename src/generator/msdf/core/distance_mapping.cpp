#include <generator/msdf/core/distance_mapping.hpp>

// msdf
#include <generator/msdf/core/range.hpp>

namespace qtxt {
    DistanceMapping DistanceMapping::Inverse(Range range) {
        double rangeWidth = range.upper - range.lower;
        return DistanceMapping(rangeWidth, range.lower / (rangeWidth ? rangeWidth : 1));
    }

    DistanceMapping::DistanceMapping() : scale{1}, translate{0} {}
    DistanceMapping::DistanceMapping(Range range) : scale{1 / (range.upper - range.lower)}, translate{-range.lower} {}

    double DistanceMapping::operator()(double d) const {
        return scale * (d + translate);
    }
    double DistanceMapping::operator()(Delta d) const {
        return scale * d.value;
    }
    DistanceMapping DistanceMapping::Inverse() const {
        return DistanceMapping(1.0 / scale, -scale * translate);
    }
}