#ifndef __QONTEXT_RECTANGLE_PACKER_HPP__
#define __QONTEXT_RECTANGLE_PACKER_HPP__

// std
#include <vector>

// msdf
#include <msdf/atlas/rectangle.hpp>

namespace qtxt {
    class RectanglePacker {
        public:
            RectanglePacker();
            RectanglePacker(int width, int height);

            void expand(int width, int height);

            int pack(Rectangle* rectangles, int count);
            int pack(OrientedRectangle* rectangles, int count);
        private:
            std::vector<Rectangle> spaces;

            static int rate(int w, int h, int sw, int sh);

            void split(int index, int w, int h);
    };
}

#endif