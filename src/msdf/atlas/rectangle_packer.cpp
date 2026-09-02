#include <msdf/atlas/rectangle_packer.hpp>

// std
#include <algorithm>

// info
#include <info/properties.hpp>

// TODO - REMOVE GOTO

namespace qtxt {
    template<typename T>
    static void removeFromUnorderedVector(std::vector<T>& vector, size_t index) {
        if (index != vector.size() - 1) {
            std::swap(vector[index], vector.back());
        }
        vector.pop_back();
    }

    int RectanglePacker::rate(int w, int h, int sw, int sh) {
        return std::min(sw - w, sh - h);
    }

    RectanglePacker::RectanglePacker() : RectanglePacker(0, 0) {}

    RectanglePacker::RectanglePacker(int width, int height) {
        if (width > 0 && height > 0) {
            spaces.push_back(Rectangle{0, 0, width, height});
        }
    }

    void RectanglePacker::expand(int width, int height) {
        if (width > 0 && height > 0) {
            int oldWidth = 0, oldHeight = 0;
            for (const Rectangle& space : spaces) {
                if (space.x + space.w > oldWidth) {
                    oldWidth = space.x + space.w;
                }
                if (space.y + space.h > oldHeight) {
                    oldHeight = space.y + space.h;
                }
            }
            spaces.push_back(Rectangle{0, 0, width, height});
            split(int(spaces.size() - 1), oldWidth, oldHeight);
        }
    }

    void RectanglePacker::split(int index, int w, int h) {
        Rectangle space = spaces[index];
        removeFromUnorderedVector(spaces, index);
        Rectangle a = {space.x, space.y + h, w, space.h - h};
        Rectangle b = {space.x + w, space.y, space.w - w, h};
        if (w * (space.h - h) < h * (space.w - w)) {
            a.w = space.w;
        } else {
            b.h = space.h;
        }
        if (a.w > 0 && a.h > 0) {
            spaces.push_back(a);
        }
        if (b.w > 0 && b.h > 0) {
            spaces.push_back(b);
        }
    }

    int RectanglePacker::pack(Rectangle* rectangles, int count) {
        std::vector<int> remainingRects(count);
        for (int i = 0; i < count; ++i) {
            remainingRects[i] = i;
        }
        while (!remainingRects.empty()) {
            int bestFit = WORST_FIT;
            int bestSpace = -1;
            int bestRect = -1;
            for (size_t i = 0; i < spaces.size(); ++i) {
                const Rectangle& space = spaces[i];
                for (size_t j = 0; j < remainingRects.size(); ++j) {
                    const Rectangle& rect = rectangles[remainingRects[j]];
                    if (rect.w == space.w && rect.h == space.h) {
                        bestSpace = i;
                        bestRect = j;
                        goto BEST_FIT_FOUND;
                    }
                    if (rect.w <= space.w && rect.h <= space.h) {
                        int fit = rate(rect.w, rect.h, space.w, space.h);
                        if (fit < bestFit) {
                            bestSpace = i;
                            bestRect = j;
                            bestFit = fit;
                        }
                    }
                }
            }
            if (bestSpace < 0 || bestRect < 0) {
                break;
            }
        BEST_FIT_FOUND:
            Rectangle &rect = rectangles[remainingRects[bestRect]];
            rect.x = spaces[bestSpace].x;
            rect.y = spaces[bestSpace].y;
            split(bestSpace, rect.w, rect.h);
            removeFromUnorderedVector(remainingRects, bestRect);
        }
        return (int)remainingRects.size();
    }

    int RectanglePacker::pack(OrientedRectangle *rectangles, int count) {
        std::vector<int> remainingRects(count);
        for (int i = 0; i < count; ++i) {
            remainingRects[i] = i;
        }
        while (!remainingRects.empty()) {
            int bestFit = WORST_FIT;
            int bestSpace = -1;
            int bestRect = -1;
            bool bestRotated = false;
            for (size_t i = 0; i < spaces.size(); ++i) {
                const Rectangle& space = spaces[i];
                for (size_t j = 0; j < remainingRects.size(); ++j) {
                    const OrientedRectangle& rect = rectangles[remainingRects[j]];
                    if (rect.w == space.w && rect.h == space.h) {
                        bestSpace = i;
                        bestRect = j;
                        bestRotated = false;
                        goto BEST_FIT_FOUND;
                    }
                    if (rect.h == space.w && rect.w == space.h) {
                        bestSpace = i;
                        bestRect = j;
                        bestRotated = true;
                        goto BEST_FIT_FOUND;
                    }
                    if (rect.w <= space.w && rect.h <= space.h) {
                        int fit = rate(rect.w, rect.h, space.w, space.h);
                        if (fit < bestFit) {
                            bestSpace = i;
                            bestRect = j;
                            bestRotated = false;
                            bestFit = fit;
                        }
                    }
                    if (rect.h <= space.w && rect.w <= space.h) {
                        int fit = rate(rect.h, rect.w, space.w, space.h);
                        if (fit < bestFit) {
                            bestSpace = i;
                            bestRect = j;
                            bestRotated = true;
                            bestFit = fit;
                        }
                    }
                }
            }
            if (bestSpace < 0 || bestRect < 0) {
                break;
            }
        BEST_FIT_FOUND:
            OrientedRectangle& rect = rectangles[remainingRects[bestRect]];
            rect.x = spaces[bestSpace].x;
            rect.y = spaces[bestSpace].y;
            rect.rotated = bestRotated;
            if (bestRotated) {
                split(bestSpace, rect.h, rect.w);
            } else {
                split(bestSpace, rect.w, rect.h);
            }
            removeFromUnorderedVector(remainingRects, bestRect);
        }
        return (int)remainingRects.size();
    }
}