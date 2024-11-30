#include <generator/msdf/msdf_atlas.hpp>

// msdf
#include <generator/msdf/core/edge_coloring.hpp>
#include <generator/msdf/atlas/font_geometry.hpp>
#include <generator/msdf/atlas/glyph_generators.hpp>
#include <generator/msdf/atlas/tight_atlas_packer.hpp>

namespace qtxt {
    Image<uint8_t, 1> GenerateSDFAtlas(FontGeometry& fontGeometry, AtlasConfig config) {
        std::vector<qtxt::GlyphGeometry> glyphs = fontGeometry.GetGlyphs();

        TightAtlasPacker packer;

        const double maxCornerAngle = 3.0;
        for (auto glyph : glyphs) {
            glyph.EdgeColoring(&qtxt::EdgeColoringInkTrap, maxCornerAngle, 0);
        }

        packer.SetDimensionsConstraint(config.constraint);
        packer.SetMinimumScale(config.minScale);
        packer.SetOriginPixelAlignment(config.alignPxOrigin);
        packer.SetPixelRange(config.pxRange);

        packer.Pack(glyphs.data(), glyphs.size());

        int width = 0, height = 0;
        packer.GetDimensions(width, height);

        qtxt::AtlasGenerator<float, uint8_t, 1, &qtxt::SDFGenerator> generator(width, height);

        qtxt::GeneratorAttributes attributes;
        generator.SetAttributes(attributes);
        generator.SetThreadCount(4);

        generator.Generate(glyphs.data(), glyphs.size());

        return generator.GetStorage().image;
    }
    Image<uint8_t, 1> GenerateSDFAtlas(MSDFFont& font, Charset charset, AtlasConfig config) {
        qtxt::FontGeometry fontGeometry(font, charset);
        return GenerateSDFAtlas(fontGeometry, config);
    }
    Image<uint8_t, 1> GeneratePSDFAtlas(FontGeometry& fontGeometry, AtlasConfig config) {
        std::vector<qtxt::GlyphGeometry> glyphs = fontGeometry.GetGlyphs();

        TightAtlasPacker packer;

        const double maxCornerAngle = 3.0;
        for (auto glyph : glyphs) {
            glyph.EdgeColoring(&qtxt::EdgeColoringInkTrap, maxCornerAngle, 0);
        }

        packer.SetDimensionsConstraint(config.constraint);
        packer.SetMinimumScale(config.minScale);
        packer.SetOriginPixelAlignment(config.alignPxOrigin);
        packer.SetPixelRange(config.pxRange);

        packer.Pack(glyphs.data(), glyphs.size());

        int width = 0, height = 0;
        packer.GetDimensions(width, height);

        qtxt::AtlasGenerator<float, uint8_t, 1, &qtxt::PSDFGenerator> generator(width, height);

        qtxt::GeneratorAttributes attributes;
        generator.SetAttributes(attributes);
        generator.SetThreadCount(4);

        generator.Generate(glyphs.data(), glyphs.size());

        return generator.GetStorage().image;
    }
    Image<uint8_t, 1> GeneratePSDFAtlas(MSDFFont& font, Charset charset, AtlasConfig config) {
        qtxt::FontGeometry fontGeometry(font, charset);
        return GeneratePSDFAtlas(fontGeometry, config);
    }
    Image<uint8_t, 3> GenerateMSDFAtlas(FontGeometry& fontGeometry, AtlasConfig config) {
        std::vector<qtxt::GlyphGeometry> glyphs = fontGeometry.GetGlyphs();

        TightAtlasPacker packer;

        const double maxCornerAngle = 3.0;
        for (auto glyph : glyphs) {
            glyph.EdgeColoring(&qtxt::EdgeColoringInkTrap, maxCornerAngle, 0);
        }

        packer.SetDimensionsConstraint(config.constraint);
        packer.SetMinimumScale(config.minScale);
        packer.SetOriginPixelAlignment(config.alignPxOrigin);
        packer.SetPixelRange(config.pxRange);

        packer.Pack(glyphs.data(), glyphs.size());

        int width = 0, height = 0;
        packer.GetDimensions(width, height);

        qtxt::AtlasGenerator<float, uint8_t, 3, &qtxt::MSDFGenerator> generator(width, height);

        qtxt::GeneratorAttributes attributes;
        generator.SetAttributes(attributes);
        generator.SetThreadCount(4);

        generator.Generate(glyphs.data(), glyphs.size());

        return generator.GetStorage().image;
    }
    Image<uint8_t, 3> GenerateMSDFAtlas(MSDFFont& font, Charset charset, AtlasConfig config) {
        qtxt::FontGeometry fontGeometry(font, charset);
        return GenerateMSDFAtlas(fontGeometry, config);
    }
    Image<uint8_t, 4> GenerateMTSDFAtlas(FontGeometry& fontGeometry, AtlasConfig config) {
        std::vector<qtxt::GlyphGeometry> glyphs = fontGeometry.GetGlyphs();

        TightAtlasPacker packer;

        const double maxCornerAngle = 3.0;
        for (auto glyph : glyphs) {
            glyph.EdgeColoring(&qtxt::EdgeColoringInkTrap, maxCornerAngle, 0);
        }

        packer.SetDimensionsConstraint(config.constraint);
        packer.SetMinimumScale(config.minScale);
        packer.SetOriginPixelAlignment(config.alignPxOrigin);
        packer.SetPixelRange(config.pxRange);

        packer.Pack(glyphs.data(), glyphs.size());

        int width = 0, height = 0;
        packer.GetDimensions(width, height);

        qtxt::AtlasGenerator<float, uint8_t, 4, &qtxt::MTSDFGenerator> generator(width, height);

        qtxt::GeneratorAttributes attributes;
        generator.SetAttributes(attributes);
        generator.SetThreadCount(4);

        generator.Generate(glyphs.data(), glyphs.size());

        return generator.GetStorage().image;
    }
    Image<uint8_t, 4> GenerateMTSDFAtlas(MSDFFont& font, Charset charset, AtlasConfig config) {
        qtxt::FontGeometry fontGeometry(font, charset);
        return GenerateMTSDFAtlas(fontGeometry, config);
    }
}