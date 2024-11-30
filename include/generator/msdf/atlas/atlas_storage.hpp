#ifndef __QONTEXT_ATLAS_STORAGE_HPP__
#define __QONTEXT_ATLAS_STORAGE_HPP__

// std
#include <cstring>
#include <algorithm>

// msdf
#include <generator/msdf/core/image.hpp>
#include <generator/msdf/core/image_ref.hpp>
#include <generator/msdf/atlas/remap.hpp>
#include <generator/msdf/atlas/image_blit.hpp>

namespace qtxt {
    template<typename T, int N>
    class AtlasStorage {
        public:
            AtlasStorage() {}
            AtlasStorage(int width, int height) : image{width, height} {
                memset((T*)image, 0, sizeof(T) * N * width * height);
            }

            AtlasStorage(const AtlasStorage<T, N>& orig, int width, int height) : image{width, height} {
                memset((T*)image, 0, sizeof(T) * N * width * height);
                Blit(image, orig.image, 0, 0, 0, 0, std::min(width, orig.image.Width()), std::min(height, orig.image.Height()));
            }
            AtlasStorage(const AtlasStorage<T, N>& orig, int width, int height, const Remap* remapping, int count) : image{width, height} {
                memset((T*)image, 0, sizeof(T) * N * width * height);
                for (int i = 0; i < count; ++i) {
                    const Remap& remap = remapping[i];
                    Blit(image, orig.image, remap.target.x, remap.target.y, remap.source.x, remap.source.y, remap.width, remap.height);
                }
            }

            explicit AtlasStorage(const ImageConstRef<T, N>& image) : image{image} {}
            explicit AtlasStorage(Image<T, N>&& image) : image{(Image<T, N>&&)image} {}

            operator ImageConstRef<T, N>() const {
                return image;
            }
            operator ImageRef<T, N>() {
                return image;
            }
            operator Image<T, N>()&& {
                return (Image<T, N>()&&)image;
            }

            template<typename S>
            void Put(int x, int y, const ImageConstRef<S, N>& subImage) {
                Blit(image, subImage, x, y, 0, 0, subImage.width, subImage.height);
            }
            void Get(int x, int y, const ImageRef<T, N>& subImage) const {
                Blit(subImage, image, 0, 0, x, y, subImage.width, subImage.height);
            }

            Image<T, N> image;
        private:
            
    };
}

#endif