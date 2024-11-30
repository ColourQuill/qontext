#include <generator/msdf/core/msdf_error_correction.hpp>

#define ARTIFACT_T_EPSILON 0.01
#define PROTECTION_RADIUS_TOLERANCE 1.001

#define CLASSIFIER_FLAG_CANDIDATE 0x01
#define CLASSIFIER_FLAG_ARTIFACT 0x02

// std
#include <vector>
#include <cstring>

// msdf
#include <generator/msdf/core/range.hpp>
#include <generator/msdf/core/image.hpp>
#include <generator/msdf/core/arithmetic.hpp>
#include <generator/msdf/core/equation_solver.hpp>
#include <generator/msdf/core/edge_color.hpp>
#include <generator/msdf/core/edge_holder.hpp>
#include <generator/msdf/core/contour.hpp>
#include <generator/msdf/core/bitmap_interpolation.hpp>
#include <generator/msdf/core/edge_selectors.hpp>
#include <generator/msdf/core/contour_combiners.hpp>
#include <generator/msdf/core/shape_distance_finder.hpp>

const double qtxt::ErrorCorrectionConfig::defaultMinDeviationRatio = 1.11111111111111111;
const double qtxt::ErrorCorrectionConfig::defaultMinImproveRatio = 1.11111111111111111;

namespace qtxt {
    class MSDFErrorCorrectionClass {
        public:
            enum Flags {
                ERROR = 1,
                PROTECTED = 2
            };

            MSDFErrorCorrectionClass();
            explicit MSDFErrorCorrectionClass(const ImageRef<uint8_t, 1>& stencil, const SDFTransformation& transformation);
            void SetMinDeviationRatio(double minDeviationRatio);
            void SetMinImproveRatio(double minImproveRatio);
            void ProtectCorners(const Shape& shape);

            template<int N>
            void ProtectEdges(const ImageConstRef<float, N>& sdf);

            void ProtectAll();
            
            template<int N>
            void FindErrors(const ImageConstRef<float, N>& sdf);

            template<template<typename> class ContourCombiner, int N>
            void FindErrors(const ImageConstRef<float, N>& sdf, const Shape& shape);

            template<int N>
            void Apply(const ImageRef<float, N>& sdf) const;

            ImageConstRef<uint8_t, 1> GetStencil() const;
        private:
            ImageRef<uint8_t, 1> stencil;
            SDFTransformation transformation;
            double minDeviationRatio;
            double minImproveRatio;
    };
    class BaseArtifactClassifier {
        public:
            inline BaseArtifactClassifier(double span, bool protectedFlag) : span{span}, protectedFlag{protectedFlag} {}
            inline int RangeTest(double at, double bt, double xt, float am, float bm, float xm) const {
                if ((am > 0.5f && bm > 0.5f && xm <= 0.5f) || (am < 0.5f && bm < 0.5f && xm >= 0.5f) || (!protectedFlag && median(am, bm, xm) != xm)) {
                    double axSpan = (xt - at) * span, bxSpan = (bt - xt) * span;
                    // Check if the interpolated median's value is in the expected range based on its distance (span) from boundaries a, b.
                    if (!(xm >= am - axSpan && xm <= am + axSpan && xm >= bm - bxSpan && xm <= bm + bxSpan)) {
                        return CLASSIFIER_FLAG_CANDIDATE | CLASSIFIER_FLAG_ARTIFACT;
                    }
                    return CLASSIFIER_FLAG_CANDIDATE;
                }
                return 0;
            }
            inline bool Evaluate(double t, float m, int flags) const {
                return (flags & 2) != 0;
            }
        private:
            double span;
            bool protectedFlag;
    };

    template<template<typename> class ContourCombiner, int N>
    class ShapeDistanceChecker {
        public:
            class ArtifactClassifier : public BaseArtifactClassifier {
                public:
                    inline ArtifactClassifier(ShapeDistanceChecker* parent, const vec2& direction, double span) : BaseArtifactClassifier{span, parent->protectedFlag}, parent{parent}, direction{direction} {}

                    inline bool Evaluate(double t, float m, int flags) const {
                        if (flags & CLASSIFIER_FLAG_CANDIDATE) {
                            // Skip expensive distance evaluation if the point has already been classified as an artifact by the base classifier.
                            if (flags & CLASSIFIER_FLAG_ARTIFACT) {
                                return true;
                            }
                            vec2 tVector = t * direction;
                            float oldMSD[N], newMSD[3];
                            // Compute the color that would be currently interpolated at the artifact candidate's position.
                            vec2 sdfCoord = parent->sdfCoord + tVector;
                            Interpolate(oldMSD, parent->sdf, sdfCoord);
                            // Compute the color that would be interpolated at the artifact candidate's position if error correction was applied on the current texel.
                            double aWeight = (1 - fabs(tVector.x)) * (1 - fabs(tVector.y));
                            float aPSD = median(parent->msd[0], parent->msd[1], parent->msd[2]);
                            newMSD[0] = float(oldMSD[0] + aWeight * (aPSD - parent->msd[0]));
                            newMSD[1] = float(oldMSD[1] + aWeight * (aPSD - parent->msd[1]));
                            newMSD[2] = float(oldMSD[2] + aWeight * (aPSD - parent->msd[2]));
                            // Compute the evaluated distance (interpolated median) before and after error correction, as well as the exact shape distance.
                            float oldPSD = median(oldMSD[0], oldMSD[1], oldMSD[2]);
                            float newPSD = median(newMSD[0], newMSD[1], newMSD[2]);
                            float refPSD = float(parent->distanceMapping(parent->distanceFinder.Distance(parent->shapeCoord + tVector * parent->texelSize)));
                            // Compare the differences of the exact distance and the before and after distances.
                            return parent->minImproveRatio * fabsf(newPSD-refPSD) < double(fabsf(oldPSD - refPSD));
                        }
                        return false;
                    }
                private:
                    ShapeDistanceChecker* parent;
                    vec2 direction;
            };
            vec2 shapeCoord, sdfCoord;
            const float* msd;
            bool protectedFlag;
            inline ShapeDistanceChecker(const ImageConstRef<float, N>& sdf, const Shape& shape, const Projection& projection, DistanceMapping distanceMapping, double minImproveRatio) : distanceFinder{shape}, sdf{sdf}, distanceMapping{distanceMapping}, minImproveRatio{minImproveRatio} {
                texelSize = projection.Unproject(vec2(1.0, 1.0));
                if (shape.inverseYAxis) {
                    texelSize.y = -texelSize.y;
                }
            }
            inline ArtifactClassifier Classifier(const vec2& direction, double span) {
                return ArtifactClassifier(this, direction, span);
            }

        private:
            ShapeDistanceFinder<ContourCombiner<PerpendicularDistanceSelector>> distanceFinder;
            ImageConstRef<float, N> sdf;
            DistanceMapping distanceMapping;
            vec2 texelSize;
            double minImproveRatio;
    };

    MSDFErrorCorrectionClass::MSDFErrorCorrectionClass() { }

    MSDFErrorCorrectionClass::MSDFErrorCorrectionClass(const ImageRef<uint8_t, 1>& stencil, const SDFTransformation& transformation) : stencil{stencil}, transformation{transformation} {
        minDeviationRatio = ErrorCorrectionConfig::defaultMinDeviationRatio;
        minImproveRatio = ErrorCorrectionConfig::defaultMinImproveRatio;
        memset(stencil.pixels, 0, stencil.width * stencil.height);
    }

    void MSDFErrorCorrectionClass::SetMinDeviationRatio(double minDeviationRatio) {
        this->minDeviationRatio = minDeviationRatio;
    }

    void MSDFErrorCorrectionClass::SetMinImproveRatio(double minImproveRatio) {
        this->minImproveRatio = minImproveRatio;
    }

    void MSDFErrorCorrectionClass::ProtectCorners(const Shape& shape) {
        for (const Contour& contour : shape.contours) {
            if (!contour.edges.empty()) {
                const EdgeSegment* prevEdge = contour.edges.back();
                for (const EdgeHolder& edge : contour.edges) {
                    int commonColor = prevEdge->color & edge->color;
                    // If the color changes from prevEdge to edge, this is a corner.
                    if (!(commonColor & (commonColor - 1))) {
                        // Find the four texels that envelop the corner and mark them as protected.
                        vec2 p = transformation.Project(edge->Point(0));
                        int l = (int)floor(p.x - 0.5);
                        int b = (int)floor(p.y - 0.5);
                        if (shape.inverseYAxis) {
                            b = stencil.height - b - 2;
                        }
                        int r = l + 1;
                        int t = b + 1;
                        // Check that the positions are within bounds.
                        if (l < stencil.width && b < stencil.height && r >= 0 && t >= 0) {
                            if (l >= 0 && b >= 0) {
                                *stencil(l, b) |= (uint8_t)PROTECTED;
                            }
                            if (r < stencil.width && b >= 0) {
                                *stencil(r, b) |= (uint8_t)PROTECTED;
                            }
                            if (l >= 0 && t < stencil.height) {
                                *stencil(l, t) |= (uint8_t)PROTECTED;
                            }
                            if (r < stencil.width && t < stencil.height) {
                                *stencil(r, t) |= (uint8_t)PROTECTED;
                            }
                        }
                    }
                    prevEdge = edge;
                }
            }
        }
    }

    /// Determines if the channel contributes to an edge between the two texels a, b.
    static bool EdgeBetweenTexelsChannel(const float* a, const float* b, int channel) {
        // Find interpolation ratio t (0 < t < 1) where an edge is expected (mix(a[channel], b[channel], t) == 0.5).
        double t = (a[channel] - 0.5) / (a[channel] - b[channel]);
        if (t > 0 && t < 1) {
            // Interpolate channel values at t.
            float c[3] = {
                mix(a[0], b[0], t),
                mix(a[1], b[1], t),
                mix(a[2], b[2], t)
            };
            // This is only an edge if the zero-distance channel is the median.
            return median(c[0], c[1], c[2]) == c[channel];
        }
        return false;
    }

    /// Returns a bit mask of which channels contribute to an edge between the two texels a, b.
    static int EdgeBetweenTexels(const float* a, const float* b) {
        return (
            RED * EdgeBetweenTexelsChannel(a, b, 0) +
            GREEN * EdgeBetweenTexelsChannel(a, b, 1) +
            BLUE * EdgeBetweenTexelsChannel(a, b, 2)
        );
    }

    /// Marks texel as protected if one of its non-median channels is present in the channel mask.
    static void ProtectExtremeChannels(uint8_t* stencil, const float* msd, float m, int mask) {
        if (
            (mask & RED && msd[0] != m) ||
            (mask & GREEN && msd[1] != m) ||
            (mask & BLUE && msd[2] != m)
        ) {
            *stencil |= (uint8_t)MSDFErrorCorrectionClass::PROTECTED;
        }
    }

    template <int N>
    void MSDFErrorCorrectionClass::ProtectEdges(const ImageConstRef<float, N>& sdf) {
        float radius;
        // Horizontal texel pairs
        radius = float(PROTECTION_RADIUS_TOLERANCE * qbn::length(transformation.Unproject(vec2(transformation.distanceMapping(DistanceMapping::Delta(1)), 0))));
        for (int y = 0; y < sdf.height; ++y) {
            const float* left = sdf(0, y);
            const float* right = sdf(1, y);
            for (int x = 0; x < sdf.width - 1; ++x) {
                float lm = median(left[0], left[1], left[2]);
                float rm = median(right[0], right[1], right[2]);
                if (fabsf(lm - 0.5f) + fabsf(rm - 0.5f) < radius) {
                    int mask = EdgeBetweenTexels(left, right);
                    ProtectExtremeChannels(stencil(x, y), left, lm, mask);
                    ProtectExtremeChannels(stencil(x + 1, y), right, rm, mask);
                }
                left += N, right += N;
            }
        }
        // Vertical texel pairs
        radius = float(PROTECTION_RADIUS_TOLERANCE * qbn::length(transformation.Unproject(vec2(0, transformation.distanceMapping(DistanceMapping::Delta(1))))));
        for (int y = 0; y < sdf.height - 1; ++y) {
            const float* bottom = sdf(0, y);
            const float* top = sdf(0, y + 1);
            for (int x = 0; x < sdf.width; ++x) {
                float bm = median(bottom[0], bottom[1], bottom[2]);
                float tm = median(top[0], top[1], top[2]);
                if (fabsf(bm - 0.5f) + fabsf(tm - 0.5f) < radius) {
                    int mask = EdgeBetweenTexels(bottom, top);
                    ProtectExtremeChannels(stencil(x, y), bottom, bm, mask);
                    ProtectExtremeChannels(stencil(x, y + 1), top, tm, mask);
                }
                bottom += N, top += N;
            }
        }
        // Diagonal texel pairs
        radius = float(PROTECTION_RADIUS_TOLERANCE * qbn::length(transformation.Unproject(vec2(transformation.distanceMapping(DistanceMapping::Delta(1))))));
        for (int y = 0; y < sdf.height - 1; ++y) {
            const float* lb = sdf(0, y);
            const float* rb = sdf(1, y);
            const float* lt = sdf(0, y + 1);
            const float* rt = sdf(1, y + 1);
            for (int x = 0; x < sdf.width - 1; ++x) {
                float mlb = median(lb[0], lb[1], lb[2]);
                float mrb = median(rb[0], rb[1], rb[2]);
                float mlt = median(lt[0], lt[1], lt[2]);
                float mrt = median(rt[0], rt[1], rt[2]);
                if (fabsf(mlb - 0.5f) + fabsf(mrt - 0.5f) < radius) {
                    int mask = EdgeBetweenTexels(lb, rt);
                    ProtectExtremeChannels(stencil(x, y), lb, mlb, mask);
                    ProtectExtremeChannels(stencil(x + 1, y + 1), rt, mrt, mask);
                }
                if (fabsf(mrb - 0.5f) + fabsf(mlt - 0.5f) < radius) {
                    int mask = EdgeBetweenTexels(rb, lt);
                    ProtectExtremeChannels(stencil(x + 1, y), rb, mrb, mask);
                    ProtectExtremeChannels(stencil(x, y + 1), lt, mlt, mask);
                }
                lb += N, rb += N, lt += N, rt += N;
            }
        }
    }

    void MSDFErrorCorrectionClass::ProtectAll() {
        uint8_t* end = stencil.pixels + stencil.width * stencil.height;
        for (uint8_t* mask = stencil.pixels; mask < end; ++mask) {
            *mask |= (uint8_t)PROTECTED;
        }
    }

    /// Returns the median of the linear interpolation of texels a, b at t.
    static float InterpolatedMedian(const float* a, const float* b, double t) {
        return median(
            mix(a[0], b[0], t),
            mix(a[1], b[1], t),
            mix(a[2], b[2], t)
        );
    }
    /// Returns the median of the bilinear interpolation with the given constant, linear, and quadratic terms at t.
    static float InterpolatedMedian(const float* a, const float* l, const float* q, double t) {
        return float(median(
            t * (t * q[0] + l[0]) + a[0],
            t * (t * q[1] + l[1]) + a[1],
            t * (t * q[2] + l[2]) + a[2]
        ));
    }

    /// Determines if the interpolated median xm is an artifact.
    static bool IsArtifact(bool isProtected, double axSpan, double bxSpan, float am, float bm, float xm) {
        return (
            // For protected texels, only report an artifact if it would cause fill inversion (change between positive and negative distance).
            (!isProtected || (am > 0.5f && bm > 0.5f && xm <= 0.5f) || (am < 0.5f && bm < 0.5f && xm >= 0.5f)) &&
            // This is an artifact if the interpolated median is outside the range of possible values based on its distance from a, b.
            !(xm >= am - axSpan && xm <= am + axSpan && xm >= bm - bxSpan && xm <= bm + bxSpan)
        );
    }

    /// Checks if a linear interpolation artifact will occur at a point where two specific color channels are equal - such points have extreme median values.
    template <class ArtifactClassifier>
    static bool HasLinearArtifactInner(const ArtifactClassifier& artifactClassifier, float am, float bm, const float* a, const float* b, float dA, float dB) {
        // Find interpolation ratio t (0 < t < 1) where two color channels are equal (mix(dA, dB, t) == 0).
        double t = (double)(dA / (dA - dB));
        if (t > ARTIFACT_T_EPSILON && t < 1 - ARTIFACT_T_EPSILON) {
            // Interpolate median at t and let the classifier decide if its value indicates an artifact.
            float xm = InterpolatedMedian(a, b, t);
            return artifactClassifier.Evaluate(t, xm, artifactClassifier.RangeTest(0, 1, t, am, bm, xm));
        }
        return false;
    }

    /// Checks if a bilinear interpolation artifact will occur at a point where two specific color channels are equal - such points have extreme median values.
    template <class ArtifactClassifier>
    static bool HasDiagonalArtifactInner(const ArtifactClassifier& artifactClassifier, float am, float dm, const float* a, const float* l, const float* q, float dA, float dBC, float dD, double tEx0, double tEx1) {
        // Find interpolation ratios t (0 < t[i] < 1) where two color channels are equal.
        double t[2];
        int solutions = SolveQuadratic(t, dD - dBC + dA, dBC - dA - dA, dA);
        for (int i = 0; i < solutions; ++i) {
            // Solutions t[i] == 0 and t[i] == 1 are singularities and occur very often because two channels are usually equal at texels.
            if (t[i] > ARTIFACT_T_EPSILON && t[i] < 1 - ARTIFACT_T_EPSILON) {
                // Interpolate median xm at t.
                float xm = InterpolatedMedian(a, l, q, t[i]);
                // Determine if xm deviates too much from medians of a, d.
                int rangeFlags = artifactClassifier.RangeTest(0, 1, t[i], am, dm, xm);
                // Additionally, check xm against the interpolated medians at the local extremes tEx0, tEx1.
                double tEnd[2];
                float em[2];
                // tEx0
                if (tEx0 > 0 && tEx0 < 1) {
                    tEnd[0] = 0, tEnd[1] = 1;
                    em[0] = am, em[1] = dm;
                    tEnd[tEx0 > t[i]] = tEx0;
                    em[tEx0 > t[i]] = InterpolatedMedian(a, l, q, tEx0);
                    rangeFlags |= artifactClassifier.RangeTest(tEnd[0], tEnd[1], t[i], em[0], em[1], xm);
                }
                // tEx1
                if (tEx1 > 0 && tEx1 < 1) {
                    tEnd[0] = 0, tEnd[1] = 1;
                    em[0] = am, em[1] = dm;
                    tEnd[tEx1 > t[i]] = tEx1;
                    em[tEx1 > t[i]] = InterpolatedMedian(a, l, q, tEx1);
                    rangeFlags |= artifactClassifier.RangeTest(tEnd[0], tEnd[1], t[i], em[0], em[1], xm);
                }
                if (artifactClassifier.Evaluate(t[i], xm, rangeFlags)) {
                    return true;
                }
            }
        }
        return false;
    }

    /// Checks if a linear interpolation artifact will occur inbetween two horizontally or vertically adjacent texels a, b.
    template <class ArtifactClassifier>
    static bool HasLinearArtifact(const ArtifactClassifier& artifactClassifier, float am, const float* a, const float* b) {
        float bm = median(b[0], b[1], b[2]);
        return (
            // Out of the pair, only report artifacts for the texel further from the edge to minimize side effects.
            fabsf(am - 0.5f) >= fabsf(bm - 0.5f) && (
                // Check points where each pair of color channels meets.
                HasLinearArtifactInner(artifactClassifier, am, bm, a, b, a[1]-a[0], b[1]-b[0]) ||
                HasLinearArtifactInner(artifactClassifier, am, bm, a, b, a[2]-a[1], b[2]-b[1]) ||
                HasLinearArtifactInner(artifactClassifier, am, bm, a, b, a[0]-a[2], b[0]-b[2])
            )
        );
    }

    /// Checks if a bilinear interpolation artifact will occur inbetween two diagonally adjacent texels a, d (with b, c forming the other diagonal).
    template <class ArtifactClassifier>
    static bool HasDiagonalArtifact(const ArtifactClassifier& artifactClassifier, float am, const float* a, const float* b, const float* c, const float* d) {
        float dm = median(d[0], d[1], d[2]);
        // Out of the pair, only report artifacts for the texel further from the edge to minimize side effects.
        if (fabsf(am - 0.5f) >= fabsf(dm - 0.5f)) {
            float abc[3] = {
                a[0]-b[0]-c[0],
                a[1]-b[1]-c[1],
                a[2]-b[2]-c[2]
            };
            // Compute the linear terms for bilinear interpolation.
            float l[3] = {
                -a[0]-abc[0],
                -a[1]-abc[1],
                -a[2]-abc[2]
            };
            // Compute the quadratic terms for bilinear interpolation.
            float q[3] = {
                d[0]+abc[0],
                d[1]+abc[1],
                d[2]+abc[2]
            };
            // Compute interpolation ratios tEx (0 < tEx[i] < 1) for the local extremes of each color channel (the derivative 2*q[i]*tEx[i]+l[i] == 0).
            double tEx[3] = {
                -.5*l[0]/q[0],
                -.5*l[1]/q[1],
                -.5*l[2]/q[2]
            };
            // Check points where each pair of color channels meets.
            return (
                HasDiagonalArtifactInner(artifactClassifier, am, dm, a, l, q, a[1] - a[0], b[1] - b[0] + c[1] - c[0], d[1] - d[0], tEx[0], tEx[1]) ||
                HasDiagonalArtifactInner(artifactClassifier, am, dm, a, l, q, a[2] - a[1], b[2] - b[1] + c[2] - c[1], d[2] - d[1], tEx[1], tEx[2]) ||
                HasDiagonalArtifactInner(artifactClassifier, am, dm, a, l, q, a[0] - a[2], b[0] - b[2] + c[0] - c[2], d[0] - d[2], tEx[2], tEx[0])
            );
        }
        return false;
    }

    template <int N>
    void MSDFErrorCorrectionClass::FindErrors(const ImageConstRef<float, N>& sdf) {
        // Compute the expected deltas between values of horizontally, vertically, and diagonally adjacent texels.
        double hSpan = minDeviationRatio * qbn::length(transformation.Unproject(vec2(transformation.distanceMapping(DistanceMapping::Delta(1)), 0)));
        double vSpan = minDeviationRatio * qbn::length(transformation.Unproject(vec2(0, transformation.distanceMapping(DistanceMapping::Delta(1)))));
        double dSpan = minDeviationRatio * qbn::length(transformation.Unproject(vec2(transformation.distanceMapping(DistanceMapping::Delta(1)))));
        // Inspect all texels.
        for (int y = 0; y < sdf.height; ++y) {
            for (int x = 0; x < sdf.width; ++x) {
                const float* c = sdf(x, y);
                float cm = median(c[0], c[1], c[2]);
                bool protectedFlag = (*stencil(x, y) & PROTECTED) != 0;
                const float* l = NULL;
                const float* b = NULL;
                const float* r = NULL;
                const float* t = NULL;
                // Mark current texel c with the error flag if an artifact occurs when it's interpolated with any of its 8 neighbors.
                *stencil(x, y) |= (uint8_t) (ERROR * (
                    (x > 0 && ((l = sdf(x - 1, y)), HasLinearArtifact(BaseArtifactClassifier(hSpan, protectedFlag), cm, c, l))) ||
                    (y > 0 && ((b = sdf(x, y - 1)), HasLinearArtifact(BaseArtifactClassifier(vSpan, protectedFlag), cm, c, b))) ||
                    (x < sdf.width - 1 && ((r = sdf(x + 1, y)), HasLinearArtifact(BaseArtifactClassifier(hSpan, protectedFlag), cm, c, r))) ||
                    (y < sdf.height - 1 && ((t = sdf(x, y + 1)), HasLinearArtifact(BaseArtifactClassifier(vSpan, protectedFlag), cm, c, t))) ||
                    (x > 0 && y > 0 && HasDiagonalArtifact(BaseArtifactClassifier(dSpan, protectedFlag), cm, c, l, b, sdf(x - 1, y - 1))) ||
                    (x < sdf.width - 1 && y > 0 && HasDiagonalArtifact(BaseArtifactClassifier(dSpan, protectedFlag), cm, c, r, b, sdf(x + 1, y - 1))) ||
                    (x > 0 && y < sdf.height - 1 && HasDiagonalArtifact(BaseArtifactClassifier(dSpan, protectedFlag), cm, c, l, t, sdf(x - 1, y + 1))) ||
                    (x < sdf.width - 1 && y < sdf.height - 1 && HasDiagonalArtifact(BaseArtifactClassifier(dSpan, protectedFlag), cm, c, r, t, sdf(x + 1, y + 1)))
                ));
            }
        }
    }

    template <template <typename> class ContourCombiner, int N>
    void MSDFErrorCorrectionClass::FindErrors(const ImageConstRef<float, N>& sdf, const Shape& shape) {
        // Compute the expected deltas between values of horizontally, vertically, and diagonally adjacent texels.
        double hSpan = minDeviationRatio * qbn::length(transformation.Unproject(vec2(transformation.distanceMapping(DistanceMapping::Delta(1)), 0)));
        double vSpan = minDeviationRatio * qbn::length(transformation.Unproject(vec2(0, transformation.distanceMapping(DistanceMapping::Delta(1)))));
        double dSpan = minDeviationRatio * qbn::length(transformation.Unproject(vec2(transformation.distanceMapping(DistanceMapping::Delta(1)))));
        // OPEN MP LIKE CODE
    #ifdef MSDFGEN_USE_OPENMP
        #pragma omp parallel
    #endif
        {
            ShapeDistanceChecker<ContourCombiner, N> shapeDistanceChecker(sdf, shape, transformation, transformation.distanceMapping, minImproveRatio);
            bool rightToLeft = false;
            // Inspect all texels.
    #ifdef MSDFGEN_USE_OPENMP
            #pragma omp for
    #endif
            for (int y = 0; y < sdf.height; ++y) {
                int row = shape.inverseYAxis ? sdf.height - y - 1 : y;
                for (int col = 0; col < sdf.width; ++col) {
                    int x = rightToLeft ? sdf.width - col - 1 : col;
                    if ((*stencil(x, row) & ERROR)) {
                        continue;
                    }
                    const float* c = sdf(x, row);
                    shapeDistanceChecker.shapeCoord = transformation.Unproject(vec2(x + 0.5, y + 0.5));
                    shapeDistanceChecker.sdfCoord = vec2(x + 0.5, row + 0.5);
                    shapeDistanceChecker.msd = c;
                    shapeDistanceChecker.protectedFlag = (*stencil(x, row) & PROTECTED) != 0;
                    float cm = median(c[0], c[1], c[2]);
                    const float* l = NULL;
                    const float* b = NULL;
                    const float* r = NULL;
                    const float* t = NULL;
                    // Mark current texel c with the error flag if an artifact occurs when it's interpolated with any of its 8 neighbors.
                    *stencil(x, row) |= (uint8_t) (ERROR * (
                        (x > 0 && ((l = sdf(x - 1, row)), HasLinearArtifact(shapeDistanceChecker.Classifier(vec2(-1.0, 0.0), hSpan), cm, c, l))) ||
                        (row > 0 && ((b = sdf(x, row - 1)), HasLinearArtifact(shapeDistanceChecker.Classifier(vec2(0.0, -1.0), vSpan), cm, c, b))) ||
                        (x < sdf.width - 1 && ((r = sdf(x + 1, row)), HasLinearArtifact(shapeDistanceChecker.Classifier(vec2(1.0, 0.0), hSpan), cm, c, r))) ||
                        (row < sdf.height - 1 && ((t = sdf(x, row + 1)), HasLinearArtifact(shapeDistanceChecker.Classifier(vec2(0.0, 1.0), vSpan), cm, c, t))) ||
                        (x > 0 && row > 0 && HasDiagonalArtifact(shapeDistanceChecker.Classifier(vec2(-1.0, -1.0), dSpan), cm, c, l, b, sdf(x - 1, row - 1))) ||
                        (x < sdf.width - 1 && row > 0 && HasDiagonalArtifact(shapeDistanceChecker.Classifier(vec2(1.0, -1.0), dSpan), cm, c, r, b, sdf(x + 1, row - 1))) ||
                        (x > 0 && row < sdf.height - 1 && HasDiagonalArtifact(shapeDistanceChecker.Classifier(vec2(-1.0, 1.0), dSpan), cm, c, l, t, sdf(x - 1, row + 1))) ||
                        (x < sdf.width - 1 && row < sdf.height - 1 && HasDiagonalArtifact(shapeDistanceChecker.Classifier(vec2(1.0, 1.0), dSpan), cm, c, r, t, sdf(x + 1, row + 1)))
                    ));
                }
            }
        }
    }

    template <int N>
    void MSDFErrorCorrectionClass::Apply(const ImageRef<float, N>& sdf) const {
        int texelCount = sdf.width * sdf.height;
        const uint8_t* mask = stencil.pixels;
        float* texel = sdf.pixels;
        for (int i = 0; i < texelCount; ++i) {
            if (*mask & ERROR) {
                // Set all color channels to the median.
                float m = median(texel[0], texel[1], texel[2]);
                texel[0] = m, texel[1] = m, texel[2] = m;
            }
            ++mask;
            texel += N;
        }
    }

    ImageConstRef<uint8_t, 1> MSDFErrorCorrectionClass::GetStencil() const {
        return stencil;
    }

    template void MSDFErrorCorrectionClass::ProtectEdges(const ImageConstRef<float, 3>& sdf);
    template void MSDFErrorCorrectionClass::ProtectEdges(const ImageConstRef<float, 4>& sdf);
    template void MSDFErrorCorrectionClass::FindErrors(const ImageConstRef<float, 3>& sdf);
    template void MSDFErrorCorrectionClass::FindErrors(const ImageConstRef<float, 4>& sdf);
    template void MSDFErrorCorrectionClass::FindErrors<SimpleContourCombiner>(const ImageConstRef<float, 3>& sdf, const Shape& shape);
    template void MSDFErrorCorrectionClass::FindErrors<SimpleContourCombiner>(const ImageConstRef<float, 4>& sdf, const Shape& shape);
    template void MSDFErrorCorrectionClass::FindErrors<OverlappingContourCombiner>(const ImageConstRef<float, 3>& sdf, const Shape& shape);
    template void MSDFErrorCorrectionClass::FindErrors<OverlappingContourCombiner>(const ImageConstRef<float, 4>& sdf, const Shape& shape);
    template void MSDFErrorCorrectionClass::Apply(const ImageRef<float, 3>& sdf) const;
    template void MSDFErrorCorrectionClass::Apply(const ImageRef<float, 4>& sdf) const;

    template <int N>
    static void MSDFErrorCorrectionInner(const ImageRef<float, N>& sdf, const Shape& shape, const SDFTransformation& transformation, const MSDFGeneratorConfig& config) {
        if (config.errorCorrection.mode == ErrorCorrectionConfig::DISABLED) {
            return;
        }
        Image<uint8_t, 1> stencilBuffer;
        if (!config.errorCorrection.buffer) {
            stencilBuffer = Image<uint8_t, 1>(sdf.width, sdf.height);
        }
        ImageRef<uint8_t, 1> stencil;
        stencil.pixels = config.errorCorrection.buffer ? config.errorCorrection.buffer : stencilBuffer.GetPixels();
        stencil.width = sdf.width, stencil.height = sdf.height;
        MSDFErrorCorrectionClass ec(stencil, transformation);
        ec.SetMinDeviationRatio(config.errorCorrection.minDeviationRatio);
        ec.SetMinImproveRatio(config.errorCorrection.minImproveRatio);
        switch (config.errorCorrection.mode) {
            case ErrorCorrectionConfig::DISABLED:
            case ErrorCorrectionConfig::INDISCRIMINATE:
                break;
            case ErrorCorrectionConfig::EDGE_PRIORITY:
                ec.ProtectCorners(shape);
                ec.ProtectEdges<N>(sdf);
                break;
            case ErrorCorrectionConfig::EDGE_ONLY:
                ec.ProtectAll();
                break;
        }
        if (config.errorCorrection.distanceCheckMode == ErrorCorrectionConfig::DO_NOT_CHECK_DISTANCE || (config.errorCorrection.distanceCheckMode == ErrorCorrectionConfig::CHECK_DISTANCE_AT_EDGE && config.errorCorrection.mode != ErrorCorrectionConfig::EDGE_ONLY)) {
            ec.FindErrors<N>(sdf);
            if (config.errorCorrection.distanceCheckMode == ErrorCorrectionConfig::CHECK_DISTANCE_AT_EDGE) {
                ec.ProtectAll();
            }
        }
        if (config.errorCorrection.distanceCheckMode == ErrorCorrectionConfig::ALWAYS_CHECK_DISTANCE || config.errorCorrection.distanceCheckMode == ErrorCorrectionConfig::CHECK_DISTANCE_AT_EDGE) {
            if (config.overlapSupport) {
                ec.FindErrors<OverlappingContourCombiner, N>(sdf, shape);
            } else {
                ec.FindErrors<SimpleContourCombiner, N>(sdf, shape);
            } 
        }
        ec.Apply(sdf);
    }

    template <int N>
    static void MSDFErrorCorrectionShapeless(const ImageRef<float, N>& sdf, const SDFTransformation& transformation, double minDeviationRatio, bool protectAll) {
        Image<uint8_t, 1> stencilBuffer(sdf.width, sdf.height);
        MSDFErrorCorrectionClass ec(stencilBuffer, transformation);
        ec.SetMinDeviationRatio(minDeviationRatio);
        if (protectAll) {
            ec.ProtectAll();
        }
        ec.FindErrors<N>(sdf);
        ec.Apply(sdf);
    }

    void MSDFErrorCorrection(const ImageRef<float, 3>& sdf, const Shape& shape, const SDFTransformation& transformation, const MSDFGeneratorConfig& config) {
        MSDFErrorCorrectionInner(sdf, shape, transformation, config);
    }
    void MSDFErrorCorrection(const ImageRef<float, 4>& sdf, const Shape& shape, const SDFTransformation& transformation, const MSDFGeneratorConfig& config) {
        MSDFErrorCorrectionInner(sdf, shape, transformation, config);
    }
    void MSDFErrorCorrection(const ImageRef<float, 3>& sdf, const Shape& shape, const Projection& projection, Range range, const MSDFGeneratorConfig& config) {
        MSDFErrorCorrectionInner(sdf, shape, SDFTransformation(projection, range), config);
    }
    void MSDFErrorCorrection(const ImageRef<float, 4>& sdf, const Shape& shape, const Projection& projection, Range range, const MSDFGeneratorConfig& config) {
        MSDFErrorCorrectionInner(sdf, shape, SDFTransformation(projection, range), config);
    }

    void MSDFFastDistanceErrorCorrection(const ImageRef<float, 3>& sdf, const SDFTransformation& transformation, double minDeviationRatio) {
        MSDFErrorCorrectionShapeless(sdf, transformation, minDeviationRatio, false);
    }
    void MSDFFastDistanceErrorCorrection(const ImageRef<float, 4>& sdf, const SDFTransformation& transformation, double minDeviationRatio) {
        MSDFErrorCorrectionShapeless(sdf, transformation, minDeviationRatio, false);
    }
    void MSDFFastDistanceErrorCorrection(const ImageRef<float, 3>& sdf, const Projection& projection, Range range, double minDeviationRatio) {
        MSDFErrorCorrectionShapeless(sdf, SDFTransformation(projection, range), minDeviationRatio, false);
    }
    void MSDFFastDistanceErrorCorrection(const ImageRef<float, 4>& sdf, const Projection& projection, Range range, double minDeviationRatio) {
        MSDFErrorCorrectionShapeless(sdf, SDFTransformation(projection, range), minDeviationRatio, false);
    }
    void MSDFFastDistanceErrorCorrection(const ImageRef<float, 3>& sdf, Range pxRange, double minDeviationRatio) {
        MSDFErrorCorrectionShapeless(sdf, SDFTransformation(Projection(), pxRange), minDeviationRatio, false);
    }
    void MSDFFastDistanceErrorCorrection(const ImageRef<float, 4>& sdf, Range pxRange, double minDeviationRatio) {
        MSDFErrorCorrectionShapeless(sdf, SDFTransformation(Projection(), pxRange), minDeviationRatio, false);            
    }

    void MSDFFastEdgeErrorCorrection(const ImageRef<float, 3>& sdf, const SDFTransformation& transformation, double minDeviationRatio) {
        MSDFErrorCorrectionShapeless(sdf, transformation, minDeviationRatio, true);
    }
    void MSDFFastEdgeErrorCorrection(const ImageRef<float, 4>& sdf, const SDFTransformation& transformation, double minDeviationRatio) {
        MSDFErrorCorrectionShapeless(sdf, transformation, minDeviationRatio, true);
    }
    void MSDFFastEdgeErrorCorrection(const ImageRef<float, 3>& sdf, const Projection& projection, Range range, double minDeviationRatio) {
        MSDFErrorCorrectionShapeless(sdf, SDFTransformation(projection, range), minDeviationRatio, true);
    }
    void MSDFFastEdgeErrorCorrection(const ImageRef<float, 4>& sdf, const Projection& projection, Range range, double minDeviationRatio) {
        MSDFErrorCorrectionShapeless(sdf, SDFTransformation(projection, range), minDeviationRatio, true);
    }
    void MSDFFastEdgeErrorCorrection(const ImageRef<float, 3>& sdf, Range pxRange, double minDeviationRatio) {
        MSDFErrorCorrectionShapeless(sdf, SDFTransformation(Projection(), pxRange), minDeviationRatio, true);
    }
    void MSDFFastEdgeErrorCorrection(const ImageRef<float, 4>& sdf, Range pxRange, double minDeviationRatio) {
        MSDFErrorCorrectionShapeless(sdf, SDFTransformation(Projection(), pxRange), minDeviationRatio, true);
    }
}