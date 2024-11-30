#ifndef __QONTEXT_IMAGE_BLIT_HPP__
#define __QONTEXT_IMAGE_BLIT_HPP__

// std
#include <cstdint>

// msdf
#include <generator/msdf/core/image_ref.hpp>

namespace qtxt {
    void Blit(const ImageRef<uint8_t, 1>& dst, const ImageConstRef<uint8_t, 1>& src, int dx, int dy, int sx, int sy, int w, int h);
    void Blit(const ImageRef<uint8_t, 3>& dst, const ImageConstRef<uint8_t, 3>& src, int dx, int dy, int sx, int sy, int w, int h);
    void Blit(const ImageRef<uint8_t, 4>& dst, const ImageConstRef<uint8_t, 4>& src, int dx, int dy, int sx, int sy, int w, int h);

    void Blit(const ImageRef<float, 1>& dst, const ImageConstRef<float, 1>& src, int dx, int dy, int sx, int sy, int w, int h);
    void Blit(const ImageRef<float, 3>& dst, const ImageConstRef<float, 3>& src, int dx, int dy, int sx, int sy, int w, int h);
    void Blit(const ImageRef<float, 4>& dst, const ImageConstRef<float, 4>& src, int dx, int dy, int sx, int sy, int w, int h);

    void Blit(const ImageRef<uint8_t, 1>& dst, const ImageConstRef<float, 1>& src, int dx, int dy, int sx, int sy, int w, int h);
    void Blit(const ImageRef<uint8_t, 3>& dst, const ImageConstRef<float, 3>& src, int dx, int dy, int sx, int sy, int w, int h);
    void Blit(const ImageRef<uint8_t, 4>& dst, const ImageConstRef<float, 4>& src, int dx, int dy, int sx, int sy, int w, int h);
}

#endif