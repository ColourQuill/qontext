#ifndef __QONTEXT_IMAGE_HPP__
#define __QONTEXT_IMAGE_HPP__

// std
#include <cstddef>
#include <cstdlib>
#include <cstring>

// msdf
#include <generator/msdf/core/image_ref.hpp>

#include <iostream>

namespace qtxt {
    template<typename T, int N = 1>
    class Image {
        public:
            Image() : pixels{NULL}, w{0}, h{0} {};
            Image(int width, int height) : w{width}, h{height} {
                pixels = new T[N * w * h];
            }
            Image(const ImageConstRef<T, N>& orig) : w{orig.width}, h{orig.height} {
                pixels = new T[N * w * h];
                memcpy(pixels, orig.pixels, sizeof(T) * N * w * h);
            }
            Image(const Image<T, N>& orig) : w{orig.Width()}, h{orig.Height()} {
                pixels = new T[N * w * h];
                memcpy(pixels, orig.pixels, sizeof(T) * N * w * h);
            }
            ~Image() {
                delete[] pixels;
            }

            Image<T, N>& operator=(const ImageConstRef<T, N>& orig) {
                if (pixels != orig.pixels) {
                    delete[] pixels;
                    w = orig.width, h = orig.height;
                    pixels = new T[N * w * h];
                    memcpy(pixels, orig.pixels, sizeof(T) * N * w * h);
                }
                return *this;
            }
            Image<T, N>& operator=(const Image<T, N>& orig) {
                if (this != &orig) {
                    delete[] pixels;
                    w = orig.w, h = orig.h;
                    pixels = new T[N * w * h];
                    memcpy(pixels, orig.pixels, sizeof(T) * N * w * h);
                }
                return *this;
            }

            int Width() const {
                return w;
            }
            int Height() const {
                return h;
            }

            T* GetPixels() const {
                return pixels;
            }

            T* operator()(int x, int y) {
                return pixels + N * (w * y + x);
            }
            const T* operator()(int x, int y) const {
                return pixels + N * (w * y + x);
            }
            operator T*() {
                return pixels;
            }
            operator const T*() const {
                return pixels;
            }
            operator ImageRef<T, N>() {
                return ImageRef<T, N>(pixels, w, h);
            }
            operator ImageConstRef<T, N>() const {
                return ImageConstRef<T, N>(pixels, w, h);
            }
        private:
            T* pixels;
            int w, h;
    };
}

#endif