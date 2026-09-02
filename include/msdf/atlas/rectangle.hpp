#ifndef __QONTEXT_RECTANGLE_HPP__
#define __QONTEXT_RECTANGLE_HPP__

namespace qtxt {
    struct Rectangle {
        int x{0}, y{0}, w{0}, h{0};
    };

    struct OrientedRectangle : Rectangle {
        bool rotated;
    };
}

#endif