#include <generator/msdf/core/rasterization.hpp>

// std
#include <vector>

// msdf
#include <generator/msdf/core/arithmetic.hpp>
#include <generator/msdf/core/projection.hpp>
#include <generator/msdf/core/shape.hpp>

namespace qtxt {
    void Rasterize(const ImageRef<float, 1>& output, const Shape& shape, const Projection& projection, FillRule fillRule) {
        Scanline scanline;
        for (int y = 0; y < output.height; ++y) {
            int row = shape.inverseYAxis ? output.height - y - 1 : y;
            shape.DoScanline(scanline, projection.UnprojectY(y + 0.5));
            for (int x = 0; x < output.width; ++x) {
                *output(x, row) = (float)scanline.Filled(projection.UnprojectX(x + 0.5), fillRule);
            }
        }
    }

    void DistanceSignCorrection(const ImageRef<float, 1>& sdf, const Shape& shape, const Projection& projection, FillRule fillRule) {
        Scanline scanline;
        for (int y = 0; y < sdf.height; ++y) {
            int row = shape.inverseYAxis ? sdf.height - y - 1 : y;
            shape.DoScanline(scanline, projection.UnprojectY(y + 0.5));
            for (int x = 0; x < sdf.width; ++x) {
                bool fill = scanline.Filled(projection.UnprojectX(x + 0.5), fillRule);
                float &sd = *sdf(x, row);
                if ((sd > 0.5f) != fill) {
                    sd = 1.0f - sd;
                }
            }
        }
    }

    template <int N>
    static void MultiDistanceSignCorrection(const ImageRef<float, N>& sdf, const Shape& shape, const Projection& projection, FillRule fillRule) {
        int w = sdf.width, h = sdf.height;
        if (!(w * h)) {
            return;
        }
        Scanline scanline;
        bool ambiguous = false;
        std::vector<char> matchMap;
        matchMap.resize(w * h);
        char *match = &matchMap[0];
        for (int y = 0; y < h; ++y) {
            int row = shape.inverseYAxis ? h - y - 1 : y;
            shape.DoScanline(scanline, projection.UnprojectY(y + 0.5));
            for (int x = 0; x < w; ++x) {
                bool fill = scanline.Filled(projection.UnprojectX(x + 0.5), fillRule);
                float *msd = sdf(x, row);
                float sd = median(msd[0], msd[1], msd[2]);
                if (sd == 0.5f) {
                    ambiguous = true;
                } else if ((sd > 0.5f) != fill) {
                    msd[0] = 1.0f - msd[0];
                    msd[1] = 1.0f - msd[1];
                    msd[2] = 1.0f - msd[2];
                    *match = -1;
                } else {
                    *match = 1;
                }
                if (N >= 4 && (msd[3] > .5f) != fill) {
                    msd[3] = 1.0f - msd[3];
                }
                ++match;
            }
        }
        // This step is necessary to avoid artifacts when whole shape is inverted
        if (ambiguous) {
            match = &matchMap[0];
            for (int y = 0; y < h; ++y) {
                int row = shape.inverseYAxis ? h - y - 1 : y;
                for (int x = 0; x < w; ++x) {
                    if (!*match) {
                        int neighborMatch = 0;
                        if (x > 0) neighborMatch += *(match - 1);
                        if (x < w - 1) neighborMatch += *(match + 1);
                        if (y > 0) neighborMatch += *(match - w);
                        if (y < h - 1) neighborMatch += *(match + w);
                        if (neighborMatch < 0) {
                            float *msd = sdf(x, row);
                            msd[0] = 1.0f - msd[0];
                            msd[1] = 1.0f - msd[1];
                            msd[2] = 1.0f - msd[2];
                        }
                    }
                    ++match;
                }
            }
        }
    }

    void DistanceSignCorrection(const ImageRef<float, 3>& sdf, const Shape& shape, const Projection& projection, FillRule fillRule) {
        MultiDistanceSignCorrection(sdf, shape, projection, fillRule);
    }

    void DistanceSignCorrection(const ImageRef<float, 4>& sdf, const Shape& shape, const Projection& projection, FillRule fillRule) {
        MultiDistanceSignCorrection(sdf, shape, projection, fillRule);
    }
}