#ifndef __QONTEXT_IMAGE_HPP__
#define __QONTEXT_IMAGE_HPP__

// std
#include <cstring>

namespace qtxt {
    template<typename T, int N = 1>
    class Image {
        public:
            inline Image() : data{nullptr}, w{0}, h{0}, owned{false} {}
            inline Image(int width, int height) : w{width}, h{height}, owned{true} {
                data = new T[N * w * h];
            }
            inline Image(T* data, int width, int height) : data{data}, w{width}, h{height}, owned{false} {}
            inline Image(const Image& orig) : w{orig.w}, h{orig.h}, owned{true} {
                data = new T[N * w * h];
                std::memcpy(data, orig.data, sizeof(T) * N * w * h);
            }
            inline Image(Image&& orig) noexcept : data{orig.data}, w{orig.w}, h{orig.h}, owned{orig.owned} {
                orig.data = nullptr;
                orig.w = orig.h = 0;
                orig.owned = false;
            }
            inline ~Image() {
                if (owned) {
                    delete[] data;
                }
            }

            inline Image& operator=(const Image& orig) {
                if (this != &orig) {
                    if (owned) {
                        delete[] data;
                    }
                    w = orig.w, h = orig.h, owned = true;
                    data = new T[N * w * h];
                    memcpy(data, orig.data, sizeof(T) * N * w * h);
                }
                return *this;
            }
            inline Image& operator=(Image&& orig) noexcept {
                if (this != &orig) {
                    if (owned) {
                        delete[] data;
                    }
                    data = orig.data, w = orig.w, h = orig.h, owned = orig.owned;
                    orig.data = nullptr;
                    orig.w = orig.h = 0;
                    orig.owned = false;
                }
                return *this;
            }

            inline int width() const { return w; }
            inline int height() const { return h; }

            inline T* pixels() { return data; }
            inline const T* pixels() const { return data; }

            inline T* operator()(int x, int y) {
                return data + N * (w * y + x);
            }
            inline const T* operator()(int x, int y) const {
                return data + N * (w * y + x);
            }
        private:
            T* data;
            int w, h;
            bool owned;
    };
}

#endif