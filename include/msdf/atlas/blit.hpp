#ifndef __QONTEXT_BLIT_HPP__
#define __QONTEXT_BLIT_HPP__

// std
#include <cstdint>

// msdf
#include <msdf/core/image.hpp>

namespace qtxt {
    void blit(Image<uint8_t, 1>& dst, const Image<uint8_t, 1>& src, int dx, int dy, int sx, int sy, int w, int h);
    void blit(Image<uint8_t, 3>& dst, const Image<uint8_t, 3>& src, int dx, int dy, int sx, int sy, int w, int h);
    void blit(Image<uint8_t, 4>& dst, const Image<uint8_t, 4>& src, int dx, int dy, int sx, int sy, int w, int h);

    void blit(Image<float, 1>& dst, const Image<float, 1>& src, int dx, int dy, int sx, int sy, int w, int h);
    void blit(Image<float, 3>& dst, const Image<float, 3>& src, int dx, int dy, int sx, int sy, int w, int h);
    void blit(Image<float, 4>& dst, const Image<float, 4>& src, int dx, int dy, int sx, int sy, int w, int h);

    void blit(Image<uint8_t, 1>& dst, const Image<float, 1>& src, int dx, int dy, int sx, int sy, int w, int h);
    void blit(Image<uint8_t, 3>& dst, const Image<float, 3>& src, int dx, int dy, int sx, int sy, int w, int h);
    void blit(Image<uint8_t, 4>& dst, const Image<float, 4>& src, int dx, int dy, int sx, int sy, int w, int h);
}

#endif