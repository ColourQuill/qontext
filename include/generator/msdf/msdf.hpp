#ifndef __QONTEXT_MSDF_HPP__
#define __QONTEXT_MSDF_HPP__

// msdf
#include <generator/msdf/core/image_ref.hpp>
#include <generator/msdf/core/msdf_generator_config.hpp>

namespace qtxt {
    // forward declarations
    class Shape;
    class SDFTransformation;
    
    // Generates a conventional single-channel signed distance field
    void GenerateSDF(const ImageRef<float, 1>& output, const Shape& shape, const SDFTransformation& transformation, const GeneratorConfig& config = GeneratorConfig());

    // Generates a single-channel signed perpendicular distance field
    void GeneratePSDF(const ImageRef<float, 1>& output, const Shape& shape, const SDFTransformation& transformation, const GeneratorConfig& config = GeneratorConfig());

    // Generates a multi-channel signed distance field. Edge colors must be assigned first! (See edgeColoringSimple)
    void GenerateMSDF(const ImageRef<float, 3>& output, const Shape& shape, const SDFTransformation& transformation, const MSDFGeneratorConfig& config = MSDFGeneratorConfig());

    // Generates a multi-channel signed distance field with true distance in the alpha channel. Edge colors must be assigned first
    void GenerateMTSDF(const ImageRef<float, 4>& output, const Shape& shape, const SDFTransformation& transformation, const MSDFGeneratorConfig& config = MSDFGeneratorConfig());
}

#endif