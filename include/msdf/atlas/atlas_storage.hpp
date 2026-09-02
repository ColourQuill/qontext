#ifndef __QONTEXT_ATLAS_STORAGE_HPP__
#define __QONTEXT_ATLAS_STORAGE_HPP__

// std
#include <cstring>
#include <algorithm>

// msdf
#include <msdf/core/image.hpp>
#include <msdf/atlas/remap.hpp>
#include <msdf/atlas/blit.hpp>

namespace qtxt {
    template<typename T, int N>
    class AtlasStorage {
        public:
            AtlasStorage() = default;
            
            AtlasStorage(int width, int height) : image{width, height} {
                std::memset(image.pixels(), 0, sizeof(T) * N * width * height);
            }

            AtlasStorage(const AtlasStorage<T, N>& orig, int width, int height) : image{width, height} {
                std::memset(image.pixels(), 0, sizeof(T) * N * width * height);
                blit(image, orig.image, 0, 0, 0, 0, std::min(width, orig.image.width()), std::min(height, orig.image.height()));
            }
            
            AtlasStorage(const AtlasStorage<T, N>& orig, int width, int height, const Remap* remapping, int count) : image{width, height} {
                std::memset(image.pixels(), 0, sizeof(T) * N * width * height);
                for (int i = 0; i < count; ++i) {
                    const Remap& remap = remapping[i];
                    blit(image, orig.image, remap.target.x, remap.target.y, remap.source.x, remap.source.y, remap.width, remap.height);
                }
            }

            explicit AtlasStorage(const Image<T, N>& image) : image{image} {}
            explicit AtlasStorage(Image<T, N>&& image) : image{std::move(image)} {}

            // Conversion operators
            operator const Image<T, N>&() const {
                return image;
            }
            operator Image<T, N>&() {
                return image;
            }
            operator Image<T, N>() && {
                return std::move(image);
            }

            template<typename S>
            void put(int x, int y, const Image<S, N>& subImage) {
                blit(image, subImage, x, y, 0, 0, subImage.width(), subImage.height());
            }
            void get(int x, int y, Image<T, N>& subImage) const {
                blit(subImage, image, 0, 0, x, y, subImage.width(), subImage.height());
            }

            Image<T, N> image;
        private:
    };
}

#endif