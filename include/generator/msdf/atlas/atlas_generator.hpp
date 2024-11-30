#ifndef __QONTEXT_ATLAS_GENERATOR_HPP__
#define __QONTEXT_ATLAS_GENERATOR_HPP__

// msdf
#include <generator/msdf/core/msdf_generator_config.hpp>
#include <generator/msdf/core/image_ref.hpp>
#include <generator/msdf/atlas/remap.hpp>
#include <generator/msdf/atlas/glyph_box.hpp>
#include <generator/msdf/atlas/glyph_geometry.hpp>
#include <generator/msdf/atlas/workload.hpp>
#include <generator/msdf/atlas/atlas_storage.hpp>

namespace qtxt {
    struct GeneratorAttributes {
        MSDFGeneratorConfig config;
        bool scanlinePass = false;
    };

    template<typename T, int N>
    using GeneratorFunction = void (*)(const ImageRef<T, N>&, const GlyphGeometry&, const GeneratorAttributes&);
    
    template<typename T, typename S, int N, GeneratorFunction<T, N> GEN_FN>
    class AtlasGenerator {
        public:
            AtlasGenerator() : threadCount{1} {}
            AtlasGenerator(int width, int height) : storage{width, height}, threadCount{1} {}
            template<typename... Args>
            AtlasGenerator(int width, int height, Args... args) : storage{width, height, args...}, threadCount{1} {}
            
            void Generate(const GlyphGeometry* glyphs, int count) {
                int maxBoxArea = 0;
                for (int i = 0; i < count; ++i) {
                    GlyphBox box = glyphs[i];
                    maxBoxArea = std::max(maxBoxArea, box.rect.w * box.rect.h);
                    layout.push_back(box);
                }
                int threadBufferSize = N * maxBoxArea;
                if (threadCount * threadBufferSize > (int)glyphBuffer.size()) {
                    glyphBuffer.resize(threadCount * threadBufferSize);
                }
                if (threadCount * maxBoxArea > (int)errorCorrectionBuffer.size()) {
                    errorCorrectionBuffer.resize(threadCount * maxBoxArea);
                }
                std::vector<GeneratorAttributes> threadAttributes(threadCount);
                for (int i = 0; i < threadCount; ++i) {
                    threadAttributes[i] = attributes;
                    threadAttributes[i].config.errorCorrection.buffer = errorCorrectionBuffer.data() + i * maxBoxArea;
                }

                Workload([this, glyphs, &threadAttributes, threadBufferSize](int i, int threadNo) -> bool {
                    const GlyphGeometry& glyph = glyphs[i];
                    if (!glyph.isWhitespace()) {
                        int l, b, w, h;
                        glyph.GetBoxRect(l, b, w, h);
                        Image<T, N> glyphImage(w, h);
                        GEN_FN(glyphImage, glyph, threadAttributes[threadNo]);
                        // for (int y = 0; y < glyphImage.Height(); ++y) {
                        //     for (int x = 0; x < glyphImage.Width(); ++x) {
                        //         auto point = glyphImage(x, y);
                        //         std::cout << "(" << point[0] << ", " << point[1]  << ", " << point[2] << ")" << std::endl;
                        //     }
                        // }
                        storage.Put(l, b, ImageConstRef<T, N>(glyphImage));
                    }
                    return true;
                }, count).Finish(threadCount);
            }
            void Rearrange(int width, int height, const Remap* remapping, int count) {
                for (int i = 0; i < count; ++i) {
                    layout[remapping[i].index].rect.x = remapping[i].target.x;
                    layout[remapping[i].index].rect.y = remapping[i].target.y;
                }
                AtlasStorage newStorage(storage, width, height, remapping, count);
                storage = newStorage;
            }
            void Resize(int width, int height) {
                AtlasStorage newStorage(storage, width, height);
                storage = newStorage;
            }

            void SetAttributes(const GeneratorAttributes& attributes) {
                this->attributes = attributes;
            }
            void SetThreadCount(int threadCount) {
                this->threadCount = threadCount;
            }

            const AtlasStorage<S, N>& GetStorage() const {
                return storage;
            }
            const std::vector<GlyphBox>& GetLayout() const {
                return layout;
            }
        private:
            AtlasStorage<S, N> storage;
            std::vector<GlyphBox> layout;
            std::vector<T> glyphBuffer;
            std::vector<uint8_t> errorCorrectionBuffer;
            GeneratorAttributes attributes;
            int threadCount;

    };
}

#endif