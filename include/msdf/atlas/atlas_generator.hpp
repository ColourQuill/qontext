#ifndef __QONTEXT_ATLAS_GENERATOR_HPP__
#define __QONTEXT_ATLAS_GENERATOR_HPP__

// msdf
#include <msdf/core/msdf_config.hpp>
#include <msdf/atlas/remap.hpp>
#include <msdf/atlas/glyph_box.hpp>
#include <msdf/atlas/glyph_geometry.hpp>
#include <msdf/atlas/workload.hpp>
#include <msdf/atlas/atlas_storage.hpp>

namespace qtxt {
    struct GeneratorAttributes {
        MSDFConfig config;
        bool scanlinePass = false;
    };

    template<typename T, int N>
    using GeneratorFunction = void (*)(Image<T, N>&, const GlyphGeometry&, const GeneratorAttributes&);
    
    template<typename T, typename S, int N, GeneratorFunction<T, N> GEN_FN>
    class AtlasGenerator {
        public:
            AtlasGenerator() : threadCount{1} {}
            AtlasGenerator(int width, int height) : storage{width, height}, threadCount{1} {}
            template<typename... Args>
            AtlasGenerator(int width, int height, Args... args) : storage{width, height, args...}, threadCount{1} {}
            
            void generate(std::vector<qtxt::GlyphGeometry*>& glyphs) {
                int maxBoxArea = 0;
                for (int i = 0; i < glyphs.size(); ++i) {
                    GlyphBox box = *glyphs[i];
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
                    const GlyphGeometry* glyph = glyphs[i];
                    if (!glyph->isWhitespace()) {
                        int l, b, w, h;
                        glyph->getBoxRect(l, b, w, h);
                        Image<T, N> glyphImage(w, h);
                        GEN_FN(glyphImage, *glyph, threadAttributes[threadNo]);
                        storage.put(l, b, glyphImage);
                    }
                    return true;
                }, glyphs.size()).finish(threadCount);
            }
            void rearrange(int width, int height, const Remap* remapping, int count) {
                for (int i = 0; i < count; ++i) {
                    layout[remapping[i].index].rect.x = remapping[i].target.x;
                    layout[remapping[i].index].rect.y = remapping[i].target.y;
                }
                AtlasStorage newStorage(storage, width, height, remapping, count);
                storage = newStorage;
            }
            void resize(int width, int height) {
                AtlasStorage newStorage(storage, width, height);
                storage = newStorage;
            }

            void setAttributes(const GeneratorAttributes& attributes) {
                this->attributes = attributes;
            }
            void setThreadCount(int threadCount) {
                this->threadCount = threadCount;
            }

            const AtlasStorage<S, N>& getStorage() const {
                return storage;
            }
            const std::vector<GlyphBox>& getLayout() const {
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