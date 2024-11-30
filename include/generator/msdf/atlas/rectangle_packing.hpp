#ifndef __QONTEXT_RECTANGLE_PACKING_HPP__
#define __QONTEXT_RECTANGLE_PACKING_HPP__

// std
#include <utility>

// msdf
#include <generator/msdf/atlas/rectangle_packer.hpp>

namespace qtxt {
    static void CopyRectanglePlacement(Rectangle& dst, const Rectangle& src) {
        dst.x = src.x;
        dst.y = src.y;
    }

    static void CopyRectanglePlacement(OrientedRectangle& dst, const OrientedRectangle& src) {
        dst.x = src.x;
        dst.y = src.y;
        dst.rotated = src.rotated;
    }

    template <typename RectangleType>
    int PackRectangles(RectangleType* rectangles, int count, int width, int height, int spacing = 0) {
        if (spacing) {
            for (int i = 0; i < count; ++i) {
                rectangles[i].w += spacing;
                rectangles[i].h += spacing;
            }
        }
        int result = RectanglePacker(width + spacing, height + spacing).Pack(rectangles, count);
        if (spacing) {
            for (int i = 0; i < count; ++i) {
                rectangles[i].w -= spacing;
                rectangles[i].h -= spacing;
            }
        }
        return result;
    }

    template <class SizeSelector, typename RectangleType>
    std::pair<int, int> PackRectangles(RectangleType* rectangles, int count, int spacing = 0) {
        std::vector<RectangleType> rectanglesCopy(count);
        int totalArea = 0;
        for (int i = 0; i < count; ++i) {
            rectanglesCopy[i].w = rectangles[i].w + spacing;
            rectanglesCopy[i].h = rectangles[i].h + spacing;
            totalArea += rectangles[i].w * rectangles[i].h;
        }
        std::pair<int, int> dimensions;
        SizeSelector sizeSelector(totalArea);
        int width, height;
        while (sizeSelector(width, height)) {
            if (!RectanglePacker(width + spacing, height + spacing).Pack(rectanglesCopy.data(), count)) {
                dimensions.first = width;
                dimensions.second = height;
                for (int i = 0; i < count; ++i) {
                    CopyRectanglePlacement(rectangles[i], rectanglesCopy[i]);
                }
                --sizeSelector;
            } else {
                ++sizeSelector;
            }
        }
        return dimensions;
    }
}

#endif