#ifndef __QONTEXT_REMAP_HPP__
#define __QONTEXT_REMAP_HPP__

namespace qtxt {
    struct Remap {
        int index;
        struct {
            int x, y;
        } source, target;
        int width, height;
    };
}

#endif