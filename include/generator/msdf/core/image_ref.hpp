#ifndef __QONTEXT_IMAGE_REF_HPP__
#define __QONTEXT_IMAGE_REF_HPP__

// std
#include <cstddef>

namespace qtxt {
    template<typename T, int N = 1>
    struct ImageRef {
        T* pixels;
        int width, height;

        inline ImageRef() : pixels{NULL}, width{0}, height{0} {}
        inline ImageRef(T* pixels, int width, int height) : pixels{pixels}, width{width}, height{height} {}

        inline T* operator()(int x, int y) const {
            return pixels + N * (width * y + x);
        }
    };

    template<typename T, int N = 1>
    struct ImageConstRef {
        const T* pixels;
        int width, height;

        inline ImageConstRef() : pixels{NULL}, width{0}, height{0} {}
        inline ImageConstRef(const T* pixels, int width, int height) : pixels{pixels}, width{width}, height{height} {}
        inline ImageConstRef(const ImageRef<T, N>& orig) : pixels{orig.pixels}, width{orig.width}, height{orig.height} {}

        inline const T* operator()(int x, int y) const {
            return pixels + N * (width * y + x);
        }
    };
}

#endif