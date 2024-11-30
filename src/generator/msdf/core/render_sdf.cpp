#include <generator/msdf/core/render_sdf.hpp>

// msdf
#include <generator/msdf/core/arithmetic.hpp>
#include <generator/msdf/core/distance_mapping.hpp>
#include <generator/msdf/core/pixel_conversion.hpp>
#include <generator/msdf/core/bitmap_interpolation.hpp>

using vec2 = qbn::vec2<double>;

namespace qtxt {
    static float DistVal(float dist, DistanceMapping mapping) {
        return (float)clamp(mapping(dist) + 0.5);
    }

    void RenderSDF(const ImageRef<float, 1>& output, const ImageConstRef<float, 1>& sdf, Range sdfPxRange, float sdThreshold) {
        vec2 scale((double)sdf.width / output.width, (double)sdf.height / output.height);
        if (sdfPxRange.lower == sdfPxRange.upper) {
            for (int y = 0; y < output.height; ++y) {
                for (int x = 0; x < output.width; ++x) {
                    float sd;
                    Interpolate(&sd, sdf, scale * vec2(x + 0.5, y + 0.5));
                    *output(x, y) = float(sd >= sdThreshold);
                }
            }
        } else {
            sdfPxRange *= (double)((output.width + output.height) / (sdf.width + sdf.height));
            DistanceMapping distanceMapping = DistanceMapping::Inverse(sdfPxRange);
            float sdBias = 0.5f - sdThreshold;
            for (int y = 0; y < output.height; ++y) {
                for (int x = 0; x < output.width; ++x) {
                    float sd;
                    Interpolate(&sd, sdf, scale * vec2(x + 0.5, y + 0.5));
                    *output(x, y) = DistVal(sd + sdBias, distanceMapping);
                }
            }
        }

    }

    void RenderSDF(const ImageRef<float, 3>& output, const ImageConstRef<float, 1>& sdf, Range sdfPxRange, float sdThreshold) {
        vec2 scale((double)sdf.width / output.width, (double)sdf.height / output.height);
        if (sdfPxRange.lower == sdfPxRange.upper) {
            for (int y = 0; y < output.height; ++y) {
                for (int x = 0; x < output.width; ++x) {
                    float sd;
                    Interpolate(&sd, sdf, scale * vec2(x + 0.5, y + 0.5));
                    float v = float(sd >= sdThreshold);
                    output(x, y)[0] = v;
                    output(x, y)[1] = v;
                    output(x, y)[2] = v;
                }
            }
        } else {
            sdfPxRange *= (double)((output.width + output.height) / (sdf.width + sdf.height));
            DistanceMapping distanceMapping = DistanceMapping::Inverse(sdfPxRange);
            float sdBias = 0.5f - sdThreshold;
            for (int y = 0; y < output.height; ++y) {
                for (int x = 0; x < output.width; ++x) {
                    float sd;
                    Interpolate(&sd, sdf, scale * vec2(x + 0.5, y + 0.5));
                    float v = DistVal(sd + sdBias, distanceMapping);
                    output(x, y)[0] = v;
                    output(x, y)[1] = v;
                    output(x, y)[2] = v;
                }
            }
        }
    }

    void RenderSDF(const ImageRef<float, 1>& output, const ImageConstRef<float, 3>& sdf, Range sdfPxRange, float sdThreshold) {
        vec2 scale((double)sdf.width / output.width, (double)sdf.height / output.height);
        if (sdfPxRange.lower == sdfPxRange.upper) {
            for (int y = 0; y < output.height; ++y) {
                for (int x = 0; x < output.width; ++x) {
                    float sd[3];
                    Interpolate(sd, sdf, scale * vec2(x + 0.5, y + 0.5));
                    *output(x, y) = float(median(sd[0], sd[1], sd[2]) >= sdThreshold);
                }
            }
        } else {
            sdfPxRange *= (double)((output.width + output.height) / (sdf.width + sdf.height));
            DistanceMapping distanceMapping = DistanceMapping::Inverse(sdfPxRange);
            float sdBias = 0.5f - sdThreshold;
            for (int y = 0; y < output.height; ++y) {
                for (int x = 0; x < output.width; ++x) {
                    float sd[3];
                    Interpolate(sd, sdf, scale * vec2(x + 0.5, y + 0.5));
                    *output(x, y) = DistVal(median(sd[0], sd[1], sd[2]) + sdBias, distanceMapping);
                }
            }
        }
    }

    void RenderSDF(const ImageRef<float, 3>& output, const ImageConstRef<float, 3>& sdf, Range sdfPxRange, float sdThreshold) {
        vec2 scale((double)sdf.width / output.width, (double)sdf.height / output.height);
        if (sdfPxRange.lower == sdfPxRange.upper) {
            for (int y = 0; y < output.height; ++y) {
                for (int x = 0; x < output.width; ++x) {
                    float sd[3];
                    Interpolate(sd, sdf, scale * vec2(x + 0.5, y + 0.5));
                    output(x, y)[0] = float(sd[0] >= sdThreshold);
                    output(x, y)[1] = float(sd[1] >= sdThreshold);
                    output(x, y)[2] = float(sd[2] >= sdThreshold);
                }
            }
        } else {
            sdfPxRange *= (double)((output.width + output.height) / (sdf.width + sdf.height));
            DistanceMapping distanceMapping = DistanceMapping::Inverse(sdfPxRange);
            float sdBias = 0.5f - sdThreshold;
            for (int y = 0; y < output.height; ++y) {
                for (int x = 0; x < output.width; ++x) {
                    float sd[3];
                    Interpolate(sd, sdf, scale * vec2(x + 0.5, y + 0.5));
                    output(x, y)[0] = DistVal(sd[0] + sdBias, distanceMapping);
                    output(x, y)[1] = DistVal(sd[1] + sdBias, distanceMapping);
                    output(x, y)[2] = DistVal(sd[2] + sdBias, distanceMapping);
                }
            }
        }
    }

    void RenderSDF(const ImageRef<float, 1>& output, const ImageConstRef<float, 4>& sdf, Range sdfPxRange, float sdThreshold) {
        vec2 scale((double)sdf.width / output.width, (double)sdf.height / output.height);
        if (sdfPxRange.lower == sdfPxRange.upper) {
            for (int y = 0; y < output.height; ++y) {
                for (int x = 0; x < output.width; ++x) {
                    float sd[4];
                    Interpolate(sd, sdf, scale * vec2(x + 0.5, y + 0.5));
                    *output(x, y) = float(median(sd[0], sd[1], sd[2]) >= sdThreshold);
                }
            }
        } else {
            sdfPxRange *= (double)((output.width + output.height) / (sdf.width + sdf.height));
            DistanceMapping distanceMapping = DistanceMapping::Inverse(sdfPxRange);
            float sdBias = .5f-sdThreshold;
            for (int y = 0; y < output.height; ++y) {
                for (int x = 0; x < output.width; ++x) {
                    float sd[4];
                    Interpolate(sd, sdf, scale * vec2(x + 0.5, y + 0.5));
                    *output(x, y) = DistVal(median(sd[0], sd[1], sd[2]) + sdBias, distanceMapping);
                }
            }
        }
    }

    void RenderSDF(const ImageRef<float, 4>& output, const ImageConstRef<float, 4>& sdf, Range sdfPxRange, float sdThreshold) {
        vec2 scale((double)sdf.width / output.width, (double)sdf.height / output.height);
        if (sdfPxRange.lower == sdfPxRange.upper) {
            for (int y = 0; y < output.height; ++y) {
                for (int x = 0; x < output.width; ++x) {
                    float sd[4];
                    Interpolate(sd, sdf, scale * vec2(x + 0.5, y + 0.5));
                    output(x, y)[0] = float(sd[0] >= sdThreshold);
                    output(x, y)[1] = float(sd[1] >= sdThreshold);
                    output(x, y)[2] = float(sd[2] >= sdThreshold);
                    output(x, y)[3] = float(sd[3] >= sdThreshold);
                }
            }
        } else {
            sdfPxRange *= (double)((output.width + output.height) / (sdf.width + sdf.height));
            DistanceMapping distanceMapping = DistanceMapping::Inverse(sdfPxRange);
            float sdBias = .5f-sdThreshold;
            for (int y = 0; y < output.height; ++y) {
                for (int x = 0; x < output.width; ++x) {
                    float sd[4];
                    Interpolate(sd, sdf, scale * vec2(x + 0.5, y + 0.5));
                    output(x, y)[0] = DistVal(sd[0] + sdBias, distanceMapping);
                    output(x, y)[1] = DistVal(sd[1] + sdBias, distanceMapping);
                    output(x, y)[2] = DistVal(sd[2] + sdBias, distanceMapping);
                    output(x, y)[3] = DistVal(sd[3] + sdBias, distanceMapping);
                }
            }
        }
    }

    void Simulate8Bit(const ImageRef<float, 1>& bitmap) {
        const float *end = bitmap.pixels + 1 * bitmap.width * bitmap.height;
        for (float *p = bitmap.pixels; p < end; ++p) {
            *p = PixelByteToFloat(PixelFloatToByte(*p));
        }
    }

    void Simulate8Bit(const ImageRef<float, 3>& bitmap) {
        const float *end = bitmap.pixels + 3 * bitmap.width * bitmap.height;
        for (float *p = bitmap.pixels; p < end; ++p) {
            *p = PixelByteToFloat(PixelFloatToByte(*p));
        }
    }

    void Simulate8Bit(const ImageRef<float, 4>& bitmap) {
        const float *end = bitmap.pixels + 4 * bitmap.width * bitmap.height;
        for (float *p = bitmap.pixels; p < end; ++p) {
            *p = PixelByteToFloat(PixelFloatToByte(*p));
        }
    }
}