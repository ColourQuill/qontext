#include <msdf/atlas/glyph_generator.hpp>

// info
#include <info/properties.hpp>

// msdf
#include <msdf/core/transformation.hpp>
#include <msdf/core/generate_msdf.hpp>
#include <msdf/core/error_correction.hpp>
#include <msdf/core/rasterization.hpp>

namespace qtxt {
    void ScanlineGenerator(Image<float, 1>& output, const GlyphGeometry& glyph, const GeneratorAttributes& attribs) {

    }
    void GlyphGenerator(Image<float, 3>& output, const GlyphGeometry& glyph, const GeneratorAttributes& attribs) {
        MSDFConfig config = attribs.config;
        if (attribs.scanlinePass) {
            config.errorCorrection.mode = ErrorCorrectionConfig::DISABLED;
        }
        auto proj = glyph.getBoxProjection();
        auto range = glyph.getBoxRange();
        Transformation transformation = Transformation(glyph.getBoxProjection(), glyph.getBoxRange());
        auto& shape = glyph.getShape();
        generateMSDF(output, shape, transformation, config);
        if (attribs.scanlinePass) {
            distanceSignCorrection(output, glyph.getShape(), glyph.getBoxProjection(), MSDF_ATLAS_GLYPH_FILL_RULE);
            if (attribs.config.errorCorrection.mode != ErrorCorrectionConfig::DISABLED) {
                config.errorCorrection.mode = attribs.config.errorCorrection.mode;
                config.errorCorrection.distanceCheckMode = ErrorCorrectionConfig::DO_NOT_CHECK_DISTANCE;
                errorCorrection(output, glyph.getShape(), glyph.getBoxProjection(), glyph.getBoxRange(), config);
            }
        }
    }
}