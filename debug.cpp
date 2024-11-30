#include <iostream>

#include <msdf/core/pixel_conversion.hpp>

#include <msdf/msdf_atlas.hpp>

namespace qtxt {
    template<typename T>
    static bool WriteValue(FILE* file, T value) {
        return fwrite(&value, sizeof(T), 1, file) == 1;
    }

    static bool WriteBMPHeader(FILE* file, int width, int height, int& paddedWidth) {
        paddedWidth = (3 * width + 3)&~3;
        const uint32_t bitmapStart = 54;
        const uint32_t bitmapSize = paddedWidth * height;
        const uint32_t fileSize = bitmapStart + bitmapSize;

        WriteValue<uint16_t>(file, 0x4d42u);
        WriteValue<uint32_t>(file, fileSize);
        WriteValue<uint16_t>(file, 0);
        WriteValue<uint16_t>(file, 0);
        WriteValue<uint32_t>(file, bitmapStart);

        WriteValue<uint32_t>(file, 40);
        WriteValue<int32_t>(file, width);
        WriteValue<int32_t>(file, height);
        WriteValue<uint16_t>(file, 1);
        WriteValue<uint16_t>(file, 24);
        WriteValue<uint32_t>(file, 0);
        WriteValue<uint32_t>(file, bitmapSize);
        WriteValue<uint32_t>(file, 2835);
        WriteValue<uint32_t>(file, 2835);
        WriteValue<uint32_t>(file, 0);
        WriteValue<uint32_t>(file, 0);

        return true;
    }

    static bool SaveBMP(const ImageConstRef<float, 1>& bitmap, const char* filename) {
        FILE* file = fopen(filename, "wb");
        if (!file) {
            return false;
        }

        int paddedWidth;
        WriteBMPHeader(file, bitmap.width, bitmap.height, paddedWidth);

        const uint8_t padding[4] = { };
        int padLength = paddedWidth-3*bitmap.width;
        for (int y = 0; y < bitmap.height; ++y) {
            for (int x = 0; x < bitmap.width; ++x) {
                uint8_t px = (uint8_t) PixelFloatToByte(*bitmap(x, y));
                fwrite(&px, sizeof(uint8_t), 1, file);
                fwrite(&px, sizeof(uint8_t), 1, file);
                fwrite(&px, sizeof(uint8_t), 1, file);
            }
            fwrite(padding, 1, padLength, file);
        }

        return !fclose(file);
    }
    static bool SaveBMP(const ImageConstRef<float, 3>& bitmap, const char* filename) {
        FILE* file = fopen(filename, "wb");
        if (!file) {
            return false;
        }

        int paddedWidth;
        WriteBMPHeader(file, bitmap.width, bitmap.height, paddedWidth);

        const uint8_t padding[4] = {};
        int padLength = paddedWidth - 3 * bitmap.width;
        for (int y = 0; y < bitmap.height; ++y) {
            for (int x = 0; x < bitmap.width; ++x) {
                uint8_t bgr[3] = {
                    (uint8_t)PixelFloatToByte(bitmap(x, y)[2]),
                    (uint8_t)PixelFloatToByte(bitmap(x, y)[1]),
                    (uint8_t)PixelFloatToByte(bitmap(x, y)[0])
                };
                fwrite(bgr, sizeof(uint8_t), 3, file);
            }
            fwrite(padding, 1, padLength, file);
        }

        return !fclose(file);
    }
    static bool SaveBMP(const ImageConstRef<uint8_t, 1>& bitmap, const char* filename) {
        FILE* file = fopen(filename, "wb");
        if (!file) {
            return false;
        }

        int paddedWidth;
        WriteBMPHeader(file, bitmap.width, bitmap.height, paddedWidth);

        const uint8_t padding[4] = { };
        int padLength = paddedWidth - 3 * bitmap.width;
        for (int y = 0; y < bitmap.height; ++y) {
            for (int x = 0; x < bitmap.width; ++x) {
                uint8_t px = (uint8_t)*bitmap(x, y);
                fwrite(&px, sizeof(uint8_t), 1, file);
                fwrite(&px, sizeof(uint8_t), 1, file);
                fwrite(&px, sizeof(uint8_t), 1, file);
            }
            fwrite(padding, 1, padLength, file);
        }

        return !fclose(file);
    }
    static bool SaveBMP(const ImageConstRef<uint8_t, 3>& bitmap, const char* filename) {
        FILE* file = fopen(filename, "wb");
        if (!file) {
            return false;
        }

        int paddedWidth;
        WriteBMPHeader(file, bitmap.width, bitmap.height, paddedWidth);

        const uint8_t padding[4] = { };
        int padLength = paddedWidth - 3 * bitmap.width;
        for (int y = 0; y < bitmap.height; ++y) {
            for (int x = 0; x < bitmap.width; ++x) {
                uint8_t bgr[3] = {
                    (uint8_t)bitmap(x, y)[2],
                    (uint8_t)bitmap(x, y)[1],
                    (uint8_t)bitmap(x, y)[0]
                };
                fwrite(bgr, sizeof(uint8_t), 3, file);
            }
            fwrite(padding, 1, padLength, file);
        }

        return !fclose(file);
    }
    static void ParseColoring(Shape& shape, const char* edgeAssigment) {
        unsigned c = 0, e = 0;
        if (shape.contours.size() < c) return;
        Contour* contour = &shape.contours[c];
        bool change = false;
        bool clear = true;
        for (const char* in = edgeAssigment; *in; ++in) {
            switch (*in) {
                case ',':
                if (change)
                    ++e;
                if (clear)
                    while (e < contour->edges.size()) {
                        contour->edges[e]->color = WHITE;
                        ++e;
                    }
                ++c, e = 0;
                if (shape.contours.size() <= c) return;
                contour = &shape.contours[c];
                change = false;
                clear = true;
                break;
            case '?':
                clear = false;
                break;
            case 'C': case 'M': case 'W': case 'Y': case 'c': case 'm': case 'w': case 'y':
                if (change) {
                    ++e;
                    change = false;
                }
                if (e < contour->edges.size()) {
                    contour->edges[e]->color = EdgeColor(
                        (*in == 'C' || *in == 'c')*CYAN|
                        (*in == 'M' || *in == 'm')*MAGENTA|
                        (*in == 'Y' || *in == 'y')*YELLOW|
                        (*in == 'W' || *in == 'w')*WHITE);
                    change = true;
                }
                break;
            }
        } 
    }
}

int main() {
    qtxt::MSDFFont font("resources/arial.ttf");

    auto image = qtxt::GenerateMSDFAtlas(font, qtxt::Charset::ASCII);

    qtxt::SaveBMP(image, "atlas.bmp");

    return 0;
}
