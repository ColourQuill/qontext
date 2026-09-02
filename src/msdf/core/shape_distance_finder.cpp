#include <msdf/core/shape_distance_finder.hpp>

namespace qtxt {
    ShapeDistanceFinder::ShapeDistanceFinder(const Shape& shape, bool overlapSupport)
        : overlapSupport{overlapSupport}, simple{shape}, overlapping{shape} {}

    Distance ShapeDistanceFinder::distance(const vec2& origin) {
        return overlapSupport ? overlapping.distance(origin) : simple.distance(origin);
    }
}