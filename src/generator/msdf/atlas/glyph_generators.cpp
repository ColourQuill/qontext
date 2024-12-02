#include <generator/msdf/atlas/glyph_generators.hpp>

// msdf
#include <generator/msdf/core/sdf_transformation.hpp>
#include <generator/msdf/core/rasterization.hpp>
#include <generator/msdf/core/msdf_error_correction.hpp>
#include <generator/msdf/msdf.hpp>

namespace qtxt {
    void ScanlineGenerator(const ImageRef<float, 1>& output, const GlyphGeometry& glyph, const GeneratorAttributes& attribs) {
        Rasterize(output, glyph.GetShape(), glyph.GetBoxProjection(), MSDF_ATLAS_GLYPH_FILL_RULE);
    }
    void SDFGenerator(const ImageRef<float, 1>& output, const GlyphGeometry& glyph, const GeneratorAttributes& attribs) {
        SDFTransformation transformation = SDFTransformation(glyph.GetBoxProjection(), glyph.GetBoxRange());
        GenerateSDF(output, glyph.GetShape(), transformation, attribs.config);
        if (attribs.scanlinePass) {
            DistanceSignCorrection(output, glyph.GetShape(), glyph.GetBoxProjection(), MSDF_ATLAS_GLYPH_FILL_RULE);
        }
    }
    void PSDFGenerator(const ImageRef<float, 1>& output, const GlyphGeometry& glyph, const GeneratorAttributes& attribs) {
        SDFTransformation transformation = SDFTransformation(glyph.GetBoxProjection(), glyph.GetBoxRange());
        GeneratePSDF(output, glyph.GetShape(), transformation, attribs.config);
        if (attribs.scanlinePass) {
            DistanceSignCorrection(output, glyph.GetShape(), glyph.GetBoxProjection(), MSDF_ATLAS_GLYPH_FILL_RULE);
        }
    }
    void MSDFGenerator(const ImageRef<float, 3>& output, const GlyphGeometry& glyph, const GeneratorAttributes& attribs) {
        MSDFGeneratorConfig config = attribs.config;
        if (attribs.scanlinePass) {
            config.errorCorrection.mode = ErrorCorrectionConfig::DISABLED;
        }
        auto proj = glyph.GetBoxProjection();
        auto range = glyph.GetBoxRange();
        SDFTransformation transformation = SDFTransformation(glyph.GetBoxProjection(), glyph.GetBoxRange());
        auto& shape = glyph.GetShape();
        GenerateMSDF(output, shape, transformation, config);
        if (attribs.scanlinePass) {
            DistanceSignCorrection(output, glyph.GetShape(), glyph.GetBoxProjection(), MSDF_ATLAS_GLYPH_FILL_RULE);
            if (attribs.config.errorCorrection.mode != ErrorCorrectionConfig::DISABLED) {
                config.errorCorrection.mode = attribs.config.errorCorrection.mode;
                config.errorCorrection.distanceCheckMode = ErrorCorrectionConfig::DO_NOT_CHECK_DISTANCE;
                MSDFErrorCorrection(output, glyph.GetShape(), glyph.GetBoxProjection(), glyph.GetBoxRange(), config);
            }
        }
    }
    void MTSDFGenerator(const ImageRef<float, 4>& output, const GlyphGeometry& glyph, const GeneratorAttributes& attribs) {
        MSDFGeneratorConfig config = attribs.config;
        if (attribs.scanlinePass) {
            config.errorCorrection.mode = ErrorCorrectionConfig::DISABLED;
        }
        SDFTransformation transformation = SDFTransformation(glyph.GetBoxProjection(), glyph.GetBoxRange());
        GenerateMTSDF(output, glyph.GetShape(), transformation, config);
        if (attribs.scanlinePass) {
            DistanceSignCorrection(output, glyph.GetShape(), glyph.GetBoxProjection(), MSDF_ATLAS_GLYPH_FILL_RULE);
            if (attribs.config.errorCorrection.mode != ErrorCorrectionConfig::DISABLED) {
                config.errorCorrection.mode = attribs.config.errorCorrection.mode;
                config.errorCorrection.distanceCheckMode = ErrorCorrectionConfig::DO_NOT_CHECK_DISTANCE;
                MSDFErrorCorrection(output, glyph.GetShape(), glyph.GetBoxProjection(), glyph.GetBoxRange(), config);
            }
        }
    }
}