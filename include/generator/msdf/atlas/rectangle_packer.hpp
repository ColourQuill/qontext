#ifndef __QONTEXT_RECTANGLE_PACKER_HPP__
#define __QONTEXT_RECTANGLE_PACKER_HPP__

// std
#include <vector>

// msdf
#include <generator/msdf/atlas/rectangle.hpp>

namespace qtxt {
    class RectanglePacker {
        public:
            RectanglePacker();
            RectanglePacker(int width, int height);

            void Expand(int width, int height);

            int Pack(Rectangle* rectangles, int count);
            int Pack(OrientedRectangle* rectangles, int count);
        private:
            std::vector<Rectangle> spaces;

            static int RateFit(int w, int h, int sw, int sh);

            void SplitSpace(int index, int w, int h);
    };
}

#endif