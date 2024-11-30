#ifndef __SDF_TRANSFORMATION_HPP__
#define __SDF_TRANSFORMATION_HPP__

// msdf
#include <generator/msdf/core/projection.hpp>
#include <generator/msdf/core/distance_mapping.hpp>

namespace qtxt {
    class SDFTransformation : public Projection {
        public:
            DistanceMapping distanceMapping;

            inline SDFTransformation() {}

            inline SDFTransformation(const Projection& projection, const DistanceMapping& distanceMapping) : Projection{projection}, distanceMapping{distanceMapping} {}
    };
}

#endif