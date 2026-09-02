#ifndef __QONTEXT_ERROR_CORRECTION_HPP__
#define __QONTEXT_ERROR_CORRECTION_HPP__

// std
#include <cstdint>

// info
#include <info/properties.hpp>

// msdf
#include <msdf/core/image.hpp>
#include <msdf/core/transformation.hpp>
#include <msdf/core/shape/shape.hpp>
#include <msdf/core/msdf_config.hpp>

namespace qtxt {
    class ErrorCorrector {
        public:
            enum Flags {
                ERROR = 1,
                PROTECTED = 2
            };

            ErrorCorrector();
            explicit ErrorCorrector(const Image<uint8_t, 1>& stencil, const Transformation& transformation);

            void setMinDeviationRatio(double minDeviationRatio);
            void setMinImproveRatio(double minImproveRatio);

            void protectCorners(const Shape& shape);
            void protectEdges(const Image<float, 3>& sdf);
            void protectAll();

            void find(const Image<float, 3>& sdf);
            void find(const Image<float, 3>& sdf, const Shape& shape, bool overlapSupport);

            void apply(Image<float, 3>& sdf) const;

            Image<uint8_t, 1> getStencil() const;
        private:
            Image<uint8_t, 1> stencil;
            Transformation transformation;
            double minDeviationRatio = DEFAULT_MIN_DEVIATION_RATIO;
            double minImproveRatio = DEFAULT_MIN_IMPROVE_RATIO;
    };

    void errorCorrection(Image<float, 3>& sdf, const Shape& shape, const Transformation& transformation, const MSDFConfig& config);
    void errorCorrection(Image<float, 3>& sdf, const Shape& shape, const Projection& projection, Range range, const MSDFConfig& config);

    void fastDistanceErrorCorrection(Image<float, 3>& sdf, const Transformation& transformation, double minDeviationRatio);
    void fastDistanceErrorCorrection(Image<float, 3>& sdf, const Projection& projection, Range range, double minDeviationRatio);
    void fastDistanceErrorCorrection(Image<float, 3>& sdf, Range pxRange, double minDeviationRatio);

    void fastEdgeErrorCorrection(Image<float, 3>& sdf, const Transformation& transformation, double minDeviationRatio);
    void fastEdgeErrorCorrection(Image<float, 3>& sdf, const Projection& projection, Range range, double minDeviationRatio);
    void fastEdgeErrorCorrection(Image<float, 3>& sdf, Range pxRange, double minDeviationRatio);
}

#endif