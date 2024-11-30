#include <generator/msdf/atlas/image_blit.hpp>

// std
#include <cstring>
#include <algorithm>

// msdf
#include <generator/msdf/core/pixel_conversion.hpp>

namespace qtxt {
    #define BOUND_AREA() { \
        if (dx < 0) w += dx, sx -= dx, dx = 0; \
        if (dy < 0) h += dy, sy -= dy, dy = 0; \
        if (sx < 0) w += sx, dx -= sx, sx = 0; \
        if (sy < 0) h += sy, dy -= sy, sy = 0; \
        w = std::max(0, std::min(w, std::min(dst.width - dx, src.width - sx))); \
        h = std::max(0, std::min(h, std::min(dst.height - dy, src.height - sy))); \
    }

    template<typename T, int N>
    void BlitSameType(const ImageRef<T, N>& dst, const ImageConstRef<T, N>& src, int dx, int dy, int sx, int sy, int w, int h) {
        BOUND_AREA();
        for (int y = 0; y < h; ++y) {
            memcpy(dst(dx, dy + y), src(sx, sy + y), sizeof(T) * N * w);
        }
    }

    #define BLIT_SAME_TYPE_IMPL(T, N) void Blit(const ImageRef<T, N>& dst, const ImageConstRef<T, N>& src, int dx, int dy, int sx, int sy, int w, int h) { BlitSameType(dst, src, dx, dy, sx, sy, w, h); }

    BLIT_SAME_TYPE_IMPL(uint8_t, 1);
    BLIT_SAME_TYPE_IMPL(uint8_t, 3);
    BLIT_SAME_TYPE_IMPL(uint8_t, 4);
    BLIT_SAME_TYPE_IMPL(float, 1);
    BLIT_SAME_TYPE_IMPL(float, 3);
    BLIT_SAME_TYPE_IMPL(float, 4);

    void Blit(const ImageRef<uint8_t, 1>& dst, const ImageConstRef<float, 1>& src, int dx, int dy, int sx, int sy, int w, int h) {
        BOUND_AREA();
        for (int y = 0; y < h; ++y) {
            uint8_t* dstPixel = dst(dx, dy + y);
            for (int x = 0; x < w; ++x) {
                const float* srcPixel = src(sx + x, sy + y);
                *dstPixel++ = PixelFloatToByte(*srcPixel);
            }
        }
    }

    void Blit(const ImageRef<uint8_t, 3>& dst, const ImageConstRef<float, 3>& src, int dx, int dy, int sx, int sy, int w, int h) {
        BOUND_AREA();
        for (int y = 0; y < h; ++y) {
            uint8_t* dstPixel = dst(dx, dy + y);
            for (int x = 0; x < w; ++x) {
                const float* srcPixel = src(sx + x, sy + y);
                *dstPixel++ = PixelFloatToByte(srcPixel[0]);
                *dstPixel++ = PixelFloatToByte(srcPixel[1]);
                *dstPixel++ = PixelFloatToByte(srcPixel[2]);
            }
        }
    }

    void Blit(const ImageRef<uint8_t, 4>& dst, const ImageConstRef<float, 4>& src, int dx, int dy, int sx, int sy, int w, int h) {
        BOUND_AREA();
        for (int y = 0; y < h; ++y) {
            uint8_t* dstPixel = dst(dx, dy + y);
            for (int x = 0; x < w; ++x) {
                const float* srcPixel = src(sx + x, sy + y);
                *dstPixel++ = PixelFloatToByte(srcPixel[0]);
                *dstPixel++ = PixelFloatToByte(srcPixel[1]);
                *dstPixel++ = PixelFloatToByte(srcPixel[2]);
                *dstPixel++ = PixelFloatToByte(srcPixel[3]);
            }
        }
    }
}