#include <msdf/msdf.hpp>

// std
#include <filesystem>

// msdf
#include <msdf/atlas/generate_atlas.hpp>
#include <msdf/atlas/atlas_config.hpp>
#include <msdf/atlas/font_geometry.hpp>
#include <msdf/atlas/glyph_geometry.hpp>

namespace qtxt {
    static const uint32_t ATLAS_FILE_MAGIC = 0x6664736du; // "msdf" little-endian
    static const uint32_t ATLAS_FILE_VERSION = 2;

    template<typename T>
    static bool WriteValue(FILE* file, T value) {
        return fwrite(&value, sizeof(T), 1, file) == 1;
    }
    template<typename T>
    static bool ReadValue(FILE* file, T& value) {
        return fread(&value, sizeof(T), 1, file) == 1;
    }

    bool saveFile(const Image<uint8_t, 3>& atlas, const FontGeometry& fontGeometry, const AtlasConfig& config, const std::string& filename) {
        FILE* file = fopen(filename.c_str(), "wb");
        if (!file) {
            return false;
        }

        std::vector<GlyphGeometry*> glyphPtrs = fontGeometry.getGlyphPtrs();
        const FontMetrics& metrics = fontGeometry.getMetrics();

        bool ok = true;
        ok &= WriteValue<uint32_t>(file, ATLAS_FILE_MAGIC);
        ok &= WriteValue<uint32_t>(file, ATLAS_FILE_VERSION);
        ok &= WriteValue<uint32_t>(file, (uint32_t)atlas.width());
        ok &= WriteValue<uint32_t>(file, (uint32_t)atlas.height());
        ok &= WriteValue<double>(file, metrics.emSize);
        ok &= WriteValue<double>(file, metrics.ascenderY);
        ok &= WriteValue<double>(file, metrics.descenderY);
        ok &= WriteValue<double>(file, metrics.lineHeight);
        ok &= WriteValue<double>(file, metrics.underlineY);
        ok &= WriteValue<double>(file, metrics.underlineThickness);
        ok &= WriteValue<double>(file, fontGeometry.getGeometryScale());

        ok &= WriteValue<int32_t>(file, (int32_t)config.constraint);
        ok &= WriteValue<double>(file, config.minScale);
        ok &= WriteValue<double>(file, config.pxRange);
        ok &= WriteValue<double>(file, config.maxCornerAngle);
        ok &= WriteValue<uint8_t>(file, config.alignPxOrigin ? 1 : 0);
        ok &= WriteValue<int32_t>(file, config.spacing);

        ok &= WriteValue<uint32_t>(file, (uint32_t)glyphPtrs.size());

        // Raw pixel data, no row padding, top-to-bottom, RGB order
        for (int y = 0; y < atlas.height() && ok; ++y) {
            for (int x = 0; x < atlas.width() && ok; ++x) {
                const uint8_t* px = atlas(x, y);
                ok &= fwrite(px, sizeof(uint8_t), 3, file) == 3;
            }
        }

        // Glyph table
        for (GlyphGeometry* g : glyphPtrs) {
            if (!ok) break;
            double planeL, planeB, planeR, planeT;
            double atlasL, atlasB, atlasR, atlasT;
            g->getQuadPlaneBounds(planeL, planeB, planeR, planeT);
            g->getQuadAtlasBounds(atlasL, atlasB, atlasR, atlasT);

            ok &= WriteValue<uint32_t>(file, g->getCodepoint());
            ok &= WriteValue<double>(file, g->getAdvance());
            ok &= WriteValue<double>(file, planeL);
            ok &= WriteValue<double>(file, planeB);
            ok &= WriteValue<double>(file, planeR);
            ok &= WriteValue<double>(file, planeT);
            ok &= WriteValue<double>(file, atlasL);
            ok &= WriteValue<double>(file, atlasB);
            ok &= WriteValue<double>(file, atlasR);
            ok &= WriteValue<double>(file, atlasT);
        }

        const std::map<std::pair<int, int>, int>& kerning = fontGeometry.getKerning();
        ok &= WriteValue<uint32_t>(file, (uint32_t)kerning.size());
        for (const auto& [pair, value] : kerning) {
            if (!ok) break;
            ok &= WriteValue<int32_t>(file, pair.first);
            ok &= WriteValue<int32_t>(file, pair.second);
            ok &= WriteValue<int32_t>(file, value);
        }

        return !fclose(file) && ok;
    }

    bool loadFile(const std::string& filename, MSDFAtlas& out) {
        FILE* file = fopen(filename.c_str(), "rb");
        if (!file) {
            return false;
        }

        uint32_t magic, version, width, height, glyphCount;
        bool ok = true;
        ok &= ReadValue(file, magic);
        if (!ok || magic != ATLAS_FILE_MAGIC) {
            fclose(file);
            return false;
        }
        ok &= ReadValue(file, version);
        if (!ok || version != ATLAS_FILE_VERSION) {
            fclose(file);
            return false;
        }
        ok &= ReadValue(file, width);
        ok &= ReadValue(file, height);
        ok &= ReadValue(file, out.metrics.emSize);
        ok &= ReadValue(file, out.metrics.ascenderY);
        ok &= ReadValue(file, out.metrics.descenderY);
        ok &= ReadValue(file, out.metrics.lineHeight);
        ok &= ReadValue(file, out.metrics.underlineY);
        ok &= ReadValue(file, out.metrics.underlineThickness);
        ok &= ReadValue(file, out.geometryScale);

        int32_t constraintVal = 0;
        ok &= ReadValue(file, constraintVal);
        out.constraint = (DimensionsConstraint)constraintVal;
        ok &= ReadValue(file, out.minScale);
        ok &= ReadValue(file, out.pxRange);
        ok &= ReadValue(file, out.maxCornerAngle);
        uint8_t alignByte = 0;
        ok &= ReadValue(file, alignByte);
        out.alignPxOrigin = alignByte != 0;
        ok &= ReadValue(file, out.spacing);

        ok &= ReadValue(file, glyphCount);
        if (!ok) {
            fclose(file);
            return false;
        }

        out.atlas = Image<uint8_t, 3>((int)width, (int)height);
        for (uint32_t y = 0; y < height && ok; ++y) {
            for (uint32_t x = 0; x < width && ok; ++x) {
                uint8_t* px = out.atlas((int)x, (int)y);
                ok &= fread(px, sizeof(uint8_t), 3, file) == 3;
            }
        }

        out.glyphs.clear();
        for (uint32_t i = 0; i < glyphCount && ok; ++i) {
            GlyphRecord rec;
            ok &= ReadValue(file, rec.codepoint);
            ok &= ReadValue(file, rec.advance);
            ok &= ReadValue(file, rec.planeL);
            ok &= ReadValue(file, rec.planeB);
            ok &= ReadValue(file, rec.planeR);
            ok &= ReadValue(file, rec.planeT);
            ok &= ReadValue(file, rec.atlasL);
            ok &= ReadValue(file, rec.atlasB);
            ok &= ReadValue(file, rec.atlasR);
            ok &= ReadValue(file, rec.atlasT);
            if (ok) {
                out.glyphs[rec.codepoint] = rec;
            }
        }

        uint32_t kerningCount = 0;
        ok &= ReadValue(file, kerningCount);
        out.kerning.clear();
        for (uint32_t i = 0; i < kerningCount && ok; ++i) {
            int32_t codepointA, codepointB, value;
            ok &= ReadValue(file, codepointA);
            ok &= ReadValue(file, codepointB);
            ok &= ReadValue(file, value);
            if (ok) {
                out.kerning[{codepointA, codepointB}] = value;
            }
        }

        fclose(file);
        return ok;
    }

    static MSDFAtlas build(const Image<uint8_t, 3>& atlas, const FontGeometry& fontGeometry, const AtlasConfig& config) {
        MSDFAtlas out;
        out.atlas = atlas;
        out.metrics = fontGeometry.getMetrics();
        out.geometryScale = fontGeometry.getGeometryScale();
        for (GlyphGeometry* g : fontGeometry.getGlyphPtrs()) {
            GlyphRecord rec;
            rec.codepoint = g->getCodepoint();
            rec.advance = g->getAdvance();
            g->getQuadPlaneBounds(rec.planeL, rec.planeB, rec.planeR, rec.planeT);
            g->getQuadAtlasBounds(rec.atlasL, rec.atlasB, rec.atlasR, rec.atlasT);
            out.glyphs[rec.codepoint] = rec;
        }
        out.kerning = fontGeometry.getKerning();
        out.constraint = config.constraint;
        out.minScale = config.minScale;
        out.pxRange = config.pxRange;
        out.maxCornerAngle = config.maxCornerAngle;
        out.alignPxOrigin = config.alignPxOrigin;
        out.spacing = config.spacing;
        return out;
    }

    static bool configMatches(const MSDFAtlas& cached, const AtlasConfig& config) {
        return cached.constraint == config.constraint
            && cached.minScale == config.minScale
            && cached.pxRange == config.pxRange
            && cached.maxCornerAngle == config.maxCornerAngle
            && cached.alignPxOrigin == config.alignPxOrigin
            && cached.spacing == config.spacing;
    }

    static std::string derive(const std::string& fontPath) {
        std::filesystem::path p(fontPath);
        p.replace_extension(".msdf");
        return p.string();
    }

    MSDFAtlas MSDF::load(const std::string& fontPath, Charset charset, AtlasConfig config, std::string atlasPath) {
        if (atlasPath.empty()) {
            atlasPath = derive(fontPath);
        }

        MSDFAtlas out;
        if (loadFile(atlasPath, out) && configMatches(out, config)) {
            return out;
        }

        Font font(fontPath);
        FontGeometry fontGeometry(font, charset);
        Image<uint8_t, 3> atlas = generateMSDFAtlas(fontGeometry, config);
        saveFile(atlas, fontGeometry, config, atlasPath);
        return build(atlas, fontGeometry, config);
    }
}