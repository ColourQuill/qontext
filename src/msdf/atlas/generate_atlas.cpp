#include <msdf/atlas/generate_atlas.hpp>

// std
#include <iostream>
#include <cstdio>
#include <fstream>

// msdf
#include <msdf/core/edge_coloring.hpp>
#include <msdf/atlas/font_geometry.hpp>
#include <msdf/atlas/glyph_generator.hpp>
#include <msdf/atlas/tight_atlas_packer.hpp>

namespace qtxt {
    Image<uint8_t, 3> generateMSDFAtlas(FontGeometry& fontGeometry, AtlasConfig config) {
        std::vector<qtxt::GlyphGeometry*> glyphs = fontGeometry.getGlyphPtrs();

        TightAtlasPacker packer;

        const double maxCornerAngle = 3.0;
        for (auto glyph : glyphs) {
            glyph->edgeColoring(&qtxt::edgeColoringInkTrap, maxCornerAngle, 0);
        }

        packer.setDimensionsConstraint(config.constraint);
        packer.setMinimumScale(config.minScale);
        packer.setOriginPixelAlignment(config.alignPxOrigin);
        packer.setPixelRange(config.pxRange);
        packer.setSpacing(config.spacing);

        int packResult = packer.pack(glyphs);
        int width = 0, height = 0;
        packer.getDimensions(width, height);

        qtxt::AtlasGenerator<float, uint8_t, 3, &qtxt::GlyphGenerator> generator(width, height);

        qtxt::GeneratorAttributes attributes;
        generator.setAttributes(attributes);
        generator.setThreadCount(4);

        generator.generate(glyphs);

        return generator.getStorage().image;
    }
    Image<uint8_t, 3> generateMSDFAtlas(Font& font, Charset charset, AtlasConfig config) {
        qtxt::FontGeometry fontGeometry(font, charset);
        return generateMSDFAtlas(fontGeometry, config);
    }
}