#ifndef __QONTEXT_TRANSFORMATION_HPP__
#define __QONTEXT_TRANSFORMATION_HPP__

// msdf
#include <msdf/core/projection.hpp>
#include <msdf/core/distance_mapping.hpp>

namespace qtxt {
    class Transformation : public Projection {
        public:
            DistanceMapping distanceMapping;

            inline Transformation() {}

            inline Transformation(const Projection& projection, const DistanceMapping& distanceMapping) : Projection{projection}, distanceMapping{distanceMapping} {}
    };
}

#endif