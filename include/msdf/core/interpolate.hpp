#ifndef __QONTEXT_INTERPOLATE_HPP__
#define __QONTEXT_INTERPOLATE_HPP__

// msdf
#include <msdf/core/vec2.hpp>
#include <msdf/core/arithmetic.hpp>
#include <msdf/core/image.hpp>

namespace qtxt {
    template<typename T, int N>
    static void interpolate(T* output, const Image<T, N>& image, vec2 pos) {
        pos.x -= 0.5;
        pos.y -= 0.5;
        int l = (int)floor(pos.x);
        int b = (int)floor(pos.y);
        int r = l + 1;
        int t = b + 1;
        double lr = pos.x - l;
        double bt = pos.y - b;
        l = clamp(l, image.width() - 1), r = clamp(r, image.width() - 1);
        b = clamp(b, image.height() - 1), t = clamp(t, image.height() - 1);
        for (int i = 0; i < N; ++i) {
            output[i] = mix(mix(image(l, b)[i], image(r, b)[i], lr), mix(image(l, t)[i], image(r, t)[i], lr), bt);
        }
    }
}

#endif