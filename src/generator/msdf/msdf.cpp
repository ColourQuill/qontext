#include <generator/msdf/msdf.hpp>

// std
#include <vector>
#include <iostream>

// msdf
#include <generator/msdf/core/sdf_transformation.hpp>
#include <generator/msdf/core/range.hpp>
#include <generator/msdf/core/shape.hpp>
#include <generator/msdf/core/edge_selectors.hpp>
#include <generator/msdf/core/contour_combiners.hpp>
#include <generator/msdf/core/shape_distance_finder.hpp>
#include <generator/msdf/core/distance_mapping.hpp>
#include <generator/msdf/core/msdf_error_correction.hpp>

namespace qtxt {
    template<typename DistanceType>
    class DistancePixelConversion;

    template<>
    class DistancePixelConversion<double> {
        public:
            typedef ImageRef<float, 1> ImageRefType;
            inline explicit DistancePixelConversion(DistanceMapping mapping) : mapping{mapping} {}
            inline void operator()(float* pixels, double distance) const {
                *pixels = float(mapping(distance));
            }
        private:
            DistanceMapping mapping;
    };

    template<>
    class DistancePixelConversion<MultiDistance> {
        public:
            typedef ImageRef<float, 3> ImageRefType;
            inline explicit DistancePixelConversion(DistanceMapping mapping) : mapping{mapping} {}
            inline void operator()(float* pixels, const MultiDistance& distance) const {
                pixels[0] = float(mapping(distance.r));
                pixels[1] = float(mapping(distance.g));
                pixels[2] = float(mapping(distance.b));
            }
        private:
            DistanceMapping mapping;
    };

    template<>
    class DistancePixelConversion<MultiAndTrueDistance> {
        public:
            typedef ImageRef<float, 4> ImageRefType;
            inline explicit DistancePixelConversion(DistanceMapping mapping) : mapping{mapping} {}
            inline void operator()(float* pixels, const MultiAndTrueDistance& distance) const {
                pixels[0] = float(mapping(distance.r));
                pixels[1] = float(mapping(distance.g));
                pixels[2] = float(mapping(distance.b));
                pixels[3] = float(mapping(distance.a));
            }
        private:
            DistanceMapping mapping;
    };

    template<class ContourCombiner>
    void GenerateDistanceField(const typename DistancePixelConversion<typename ContourCombiner::DistanceType>::ImageRefType& output, const Shape& shape, const SDFTransformation& transformation) {
        DistancePixelConversion<typename ContourCombiner::DistanceType> distancePixelConversion(transformation.distanceMapping);
        // parallel open mp again implement please
        {
            ShapeDistanceFinder<ContourCombiner> distanceFinder(shape);
            bool rightToLeft = false;
            for (int y = 0; y < output.height; ++y) {
                int row = shape.inverseYAxis ? output.height - y - 1 : y;
                for (int col = 0; col < output.width; ++col) {
                    int x = rightToLeft ? output.width - col - 1 : col;
                    vec2 p = transformation.Unproject(vec2(x + 0.5, y + 0.5));
                    typename ContourCombiner::DistanceType distance = distanceFinder.Distance(p);
                    distancePixelConversion(output(x, row), distance);
                }
                rightToLeft = !rightToLeft;
            }
        }
    }

    void GenerateSDF(const ImageRef<float, 1>& output, const Shape& shape, const SDFTransformation& transformation, const GeneratorConfig& config) {
        if (config.overlapSupport) {
            GenerateDistanceField<OverlappingContourCombiner<TrueDistanceSelector>>(output, shape, transformation);
        } else {
            GenerateDistanceField<SimpleContourCombiner<TrueDistanceSelector>>(output, shape, transformation);
        }
    }

    void GeneratePSDF(const ImageRef<float, 1>& output, const Shape& shape, const SDFTransformation& transformation, const GeneratorConfig& config) {
        if (config.overlapSupport) {
            GenerateDistanceField<OverlappingContourCombiner<PerpendicularDistanceSelector>>(output, shape, transformation);
        } else {
            GenerateDistanceField<SimpleContourCombiner<PerpendicularDistanceSelector>>(output, shape, transformation);
        }  
    }

    void GenerateMSDF(const ImageRef<float, 3>& output, const Shape& shape, const SDFTransformation& transformation, const MSDFGeneratorConfig& config) {
        if (config.overlapSupport) {
            GenerateDistanceField<OverlappingContourCombiner<MultiDistanceSelector>>(output, shape, transformation);
        } else {
            GenerateDistanceField<SimpleContourCombiner<MultiDistanceSelector>>(output, shape, transformation);
        }
        MSDFErrorCorrection(output, shape, transformation, config);
    }

    void GenerateMTSDF(const ImageRef<float, 4>& output, const Shape& shape, const SDFTransformation& transformation, const MSDFGeneratorConfig& config) {
        if (config.overlapSupport) {
            GenerateDistanceField<OverlappingContourCombiner<MultiAndTrueDistanceSelector>>(output, shape, transformation);
        } else {
            GenerateDistanceField<SimpleContourCombiner<MultiAndTrueDistanceSelector>>(output, shape, transformation);
        }
        MSDFErrorCorrection(output, shape, transformation, config);
    }
}