#ifndef __MSDF_ERROR_CORRECTION_HPP__
#define __MSDF_ERROR_CORRECTION_HPP__

// qarbon
#include <qarbon.hpp>

// msdf
#include <generator/msdf/core/image_ref.hpp>
#include <generator/msdf/core/sdf_transformation.hpp>
#include <generator/msdf/core/msdf_generator_config.hpp>

namespace qtxt {
    using vec2 = qbn::vec2<double>;
    
    // forward declarations
    struct Range;
    class Projection;
    class Shape;

    void MSDFErrorCorrection(const ImageRef<float, 3>& sdf, const Shape& shape, const SDFTransformation& transformation, const MSDFGeneratorConfig& config = MSDFGeneratorConfig());
    void MSDFErrorCorrection(const ImageRef<float, 4>& sdf, const Shape& shape, const SDFTransformation& transformation, const MSDFGeneratorConfig& config = MSDFGeneratorConfig());
    void MSDFErrorCorrection(const ImageRef<float, 3>& sdf, const Shape& shape, const Projection& projection, Range range, const MSDFGeneratorConfig& config = MSDFGeneratorConfig());
    void MSDFErrorCorrection(const ImageRef<float, 4>& sdf, const Shape& shape, const Projection& projection, Range range, const MSDFGeneratorConfig& config = MSDFGeneratorConfig());

    void MSDFFastDistanceErrorCorrection(const ImageRef<float, 3>& sdf, const SDFTransformation& transformation, double minDeviationRatio = ErrorCorrectionConfig::defaultMinDeviationRatio);
    void MSDFFastDistanceErrorCorrection(const ImageRef<float, 4>& sdf, const SDFTransformation& transformation, double minDeviationRatio = ErrorCorrectionConfig::defaultMinDeviationRatio);
    void MSDFFastDistanceErrorCorrection(const ImageRef<float, 3>& sdf, const Projection& projection, Range range, double minDeviationRatio = ErrorCorrectionConfig::defaultMinDeviationRatio);
    void MSDFFastDistanceErrorCorrection(const ImageRef<float, 4>& sdf, const Projection& projection, Range range, double minDeviationRatio = ErrorCorrectionConfig::defaultMinDeviationRatio);
    void MSDFFastDistanceErrorCorrection(const ImageRef<float, 3>& sdf, Range pxRange, double minDeviationRatio = ErrorCorrectionConfig::defaultMinDeviationRatio);
    void MSDFFastDistanceErrorCorrection(const ImageRef<float, 4>& sdf, Range pxRange, double minDeviationRatio = ErrorCorrectionConfig::defaultMinDeviationRatio);

    void MSDFFastEdgeErrorCorrection(const ImageRef<float, 3>& sdf, const SDFTransformation& transformation, double minDeviationRatio = ErrorCorrectionConfig::defaultMinDeviationRatio);
    void MSDFFastEdgeErrorCorrection(const ImageRef<float, 4>& sdf, const SDFTransformation& transformation, double minDeviationRatio = ErrorCorrectionConfig::defaultMinDeviationRatio);
    void MSDFFastEdgeErrorCorrection(const ImageRef<float, 3>& sdf, const Projection& projection, Range range, double minDeviationRatio = ErrorCorrectionConfig::defaultMinDeviationRatio);
    void MSDFFastEdgeErrorCorrection(const ImageRef<float, 4>& sdf, const Projection& projection, Range range, double minDeviationRatio = ErrorCorrectionConfig::defaultMinDeviationRatio);
    void MSDFFastEdgeErrorCorrection(const ImageRef<float, 3>& sdf, Range pxRange, double minDeviationRatio = ErrorCorrectionConfig::defaultMinDeviationRatio);
    void MSDFFastEdgeErrorCorrection(const ImageRef<float, 4>& sdf, Range pxRange, double minDeviationRatio = ErrorCorrectionConfig::defaultMinDeviationRatio);
}

#endif