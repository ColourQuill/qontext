#ifndef __QONTEXT_GENERATE_MSDF_HPP__
#define __QONTEXT_GENERATE_MSDF_HPP__

// msdf
#include <msdf/core/shape_distance_finder.hpp>
#include <msdf/core/error_correction.hpp>

namespace qtxt {
    void generateMSDF(Image<float, 3>& output, const Shape& shape, const Transformation& transformation, const MSDFConfig& config);
}

#endif