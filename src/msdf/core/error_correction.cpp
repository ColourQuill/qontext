#include <msdf/core/error_correction.hpp>

// msdf
#include <msdf/core/contour_combiners.hpp>
#include <msdf/core/equation_solver.hpp>
#include <msdf/core/arithmetic.hpp>
#include <msdf/core/interpolate.hpp>
#include <msdf/core/range.hpp>
#include <msdf/core/shape_distance_finder.hpp>

namespace qtxt {
    class BaseArtifactClassifier {
        public:
            inline BaseArtifactClassifier(double span, bool protectedFlag) : span{span}, protectedFlag{protectedFlag} {}
            inline int rangeTest(double at, double bt, double xt, float am, float bm, float xm) const {
                if ((am > 0.5f && bm > 0.5f && xm <= 0.5f) || (am < 0.5f && bm < 0.5f && xm >= 0.5f) || (!protectedFlag && median(am, bm, xm) != xm)) {
                    double axSpan = (xt - at) * span, bxSpan = (bt - xt) * span;
                    if (!(xm >= am - axSpan && xm <= am + axSpan && xm >= bm - bxSpan && xm <= bm + bxSpan)) {
                        return CLASSIFIER_FLAG_CANDIDATE | CLASSIFIER_FLAG_ARTIFACT;
                    }
                    return CLASSIFIER_FLAG_CANDIDATE;
                }
                return 0;
            }
            inline bool evaluate(double, float, int flags) const {
                return (flags & 2) != 0;
            }
        private:
            double span;
            bool protectedFlag;
    };

    template<class Combiner>
    class ShapeDistanceChecker {
        public:
            class ArtifactClassifier : public BaseArtifactClassifier {
                public:
                    inline ArtifactClassifier(ShapeDistanceChecker* parent, const vec2& direction, double span)
                        : BaseArtifactClassifier{span, parent->protectedFlag}, parent{parent}, direction{direction} {}

                    inline bool evaluate(double t, float, int flags) const {
                        if (flags & CLASSIFIER_FLAG_CANDIDATE) {
                            if (flags & CLASSIFIER_FLAG_ARTIFACT) {
                                return true;
                            }
                            vec2 tVector = t * direction;
                            float oldMSD[3], newMSD[3];
                            vec2 sdfCoord = parent->sdfCoord + tVector;
                            interpolate(oldMSD, parent->sdf, sdfCoord);
                            double aWeight = (1 - fabs(tVector.x)) * (1 - fabs(tVector.y));
                            float aPSD = median(parent->msd[0], parent->msd[1], parent->msd[2]);
                            newMSD[0] = float(oldMSD[0] + aWeight * (aPSD - parent->msd[0]));
                            newMSD[1] = float(oldMSD[1] + aWeight * (aPSD - parent->msd[1]));
                            newMSD[2] = float(oldMSD[2] + aWeight * (aPSD - parent->msd[2]));
                            float oldPSD = median(oldMSD[0], oldMSD[1], oldMSD[2]);
                            float newPSD = median(newMSD[0], newMSD[1], newMSD[2]);
                            float refPSD = float(parent->distanceMapping(median(
                                parent->distanceFinder.distance(parent->shapeCoord + tVector * parent->texelSize).r,
                                parent->distanceFinder.distance(parent->shapeCoord + tVector * parent->texelSize).g,
                                parent->distanceFinder.distance(parent->shapeCoord + tVector * parent->texelSize).b
                            )));
                            return parent->minImproveRatio * fabsf(newPSD - refPSD) < double(fabsf(oldPSD - refPSD));
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

            inline ShapeDistanceChecker(const Image<float, 3>& sdf, const Shape& shape, const Projection& projection, DistanceMapping distanceMapping, double minImproveRatio)
                : distanceFinder{shape,  /* TODO ADD TRUE OR FALSE INPUT */ true}, sdf{sdf}, distanceMapping{distanceMapping}, minImproveRatio{minImproveRatio} {
                texelSize = projection.unproject(vec2(1.0, 1.0));
                if (shape.inverseYAxis) {
                    texelSize.y = -texelSize.y;
                }
            }
            inline ArtifactClassifier classifier(const vec2& direction, double span) {
                return ArtifactClassifier(this, direction, span);
            }

        private:
            ShapeDistanceFinder distanceFinder;
            Image<float, 3> sdf;
            DistanceMapping distanceMapping;
            vec2 texelSize;
            double minImproveRatio;
    };

    static bool edgeBetweenTexelsChannel(const float* a, const float* b, int channel) {
        double t = (a[channel] - 0.5) / (a[channel] - b[channel]);
        if (t > 0 && t < 1) {
            float c[3] = { mix(a[0], b[0], t), mix(a[1], b[1], t), mix(a[2], b[2], t) };
            return median(c[0], c[1], c[2]) == c[channel];
        }
        return false;
    }

    static Color combineChannels(bool r, bool g, bool b) {
        if (r && g && b) return Color::WHITE;
        if (r && g) return Color::YELLOW;
        if (r && b) return Color::MAGENTA;
        if (g && b) return Color::CYAN;
        if (r) return Color::RED;
        if (g) return Color::GREEN;
        if (b) return Color::BLUE;
        return Color::BLACK;
    }

    static Color edgeBetweenTexels(const float* a, const float* b) {
        return combineChannels(
            edgeBetweenTexelsChannel(a, b, 0),
            edgeBetweenTexelsChannel(a, b, 1),
            edgeBetweenTexelsChannel(a, b, 2)
        );
    }

    static int sharedChannelCount(Color a, Color b) {
        int count = 0;
        if (hasRed(a) && hasRed(b)) ++count;
        if (hasGreen(a) && hasGreen(b)) ++count;
        if (hasBlue(a) && hasBlue(b)) ++count;
        return count;
    }

    static void protectExtremeChannels(uint8_t* stencil, const float* msd, float m, Color mask) {
        if ((hasRed(mask) && msd[0] != m) || (hasGreen(mask) && msd[1] != m) || (hasBlue(mask) && msd[2] != m)) {
            *stencil |= (uint8_t)ErrorCorrector::PROTECTED;
        }
    }

    static float interpolatedMedian(const float* a, const float* b, double t) {
        return median(mix(a[0], b[0], t), mix(a[1], b[1], t), mix(a[2], b[2], t));
    }

    static float interpolatedMedian(const float* a, const float* l, const float* q, double t) {
        return float(median(
            t * (t * q[0] + l[0]) + a[0],
            t * (t * q[1] + l[1]) + a[1],
            t * (t * q[2] + l[2]) + a[2]
        ));
    }

    template <class ArtifactClassifier>
    static bool hasLinearArtifactInner(const ArtifactClassifier& artifactClassifier, float am, float bm, const float* a, const float* b, float dA, float dB) {
        double t = (double)(dA / (dA - dB));
        if (t > ARTIFACT_T_EPSILON && t < 1 - ARTIFACT_T_EPSILON) {
            float xm = interpolatedMedian(a, b, t);
            return artifactClassifier.evaluate(t, xm, artifactClassifier.rangeTest(0, 1, t, am, bm, xm));
        }
        return false;
    }

    template <class ArtifactClassifier>
    static bool hasDiagonalArtifactInner(const ArtifactClassifier& artifactClassifier, float am, float dm, const float* a, const float* l, const float* q, float dA, float dBC, float dD, double tEx0, double tEx1) {
        double t[2];
        int solutions = solveQuadratic(t, dD - dBC + dA, dBC - dA - dA, dA);
        for (int i = 0; i < solutions; ++i) {
            if (t[i] > ARTIFACT_T_EPSILON && t[i] < 1 - ARTIFACT_T_EPSILON) {
                float xm = interpolatedMedian(a, l, q, t[i]);
                int rangeFlags = artifactClassifier.rangeTest(0, 1, t[i], am, dm, xm);
                double tEnd[2];
                float em[2];
                if (tEx0 > 0 && tEx0 < 1) {
                    tEnd[0] = 0, tEnd[1] = 1;
                    em[0] = am, em[1] = dm;
                    tEnd[tEx0 > t[i]] = tEx0;
                    em[tEx0 > t[i]] = interpolatedMedian(a, l, q, tEx0);
                    rangeFlags |= artifactClassifier.rangeTest(tEnd[0], tEnd[1], t[i], em[0], em[1], xm);
                }
                if (tEx1 > 0 && tEx1 < 1) {
                    tEnd[0] = 0, tEnd[1] = 1;
                    em[0] = am, em[1] = dm;
                    tEnd[tEx1 > t[i]] = tEx1;
                    em[tEx1 > t[i]] = interpolatedMedian(a, l, q, tEx1);
                    rangeFlags |= artifactClassifier.rangeTest(tEnd[0], tEnd[1], t[i], em[0], em[1], xm);
                }
                if (artifactClassifier.evaluate(t[i], xm, rangeFlags)) {
                    return true;
                }
            }
        }
        return false;
    }

    template <class ArtifactClassifier>
    static bool hasLinearArtifact(const ArtifactClassifier& artifactClassifier, float am, const float* a, const float* b) {
        float bm = median(b[0], b[1], b[2]);
        return (
            fabsf(am - 0.5f) >= fabsf(bm - 0.5f) && (
                hasLinearArtifactInner(artifactClassifier, am, bm, a, b, a[1]-a[0], b[1]-b[0]) ||
                hasLinearArtifactInner(artifactClassifier, am, bm, a, b, a[2]-a[1], b[2]-b[1]) ||
                hasLinearArtifactInner(artifactClassifier, am, bm, a, b, a[0]-a[2], b[0]-b[2])
            )
        );
    }

    template <class ArtifactClassifier>
    static bool hasDiagonalArtifact(const ArtifactClassifier& artifactClassifier, float am, const float* a, const float* b, const float* c, const float* d) {
        float dm = median(d[0], d[1], d[2]);
        if (fabsf(am - 0.5f) >= fabsf(dm - 0.5f)) {
            float abc[3] = { a[0]-b[0]-c[0], a[1]-b[1]-c[1], a[2]-b[2]-c[2] };
            float l[3] = { -a[0]-abc[0], -a[1]-abc[1], -a[2]-abc[2] };
            float q[3] = { d[0]+abc[0], d[1]+abc[1], d[2]+abc[2] };
            double tEx[3] = { -.5*l[0]/q[0], -.5*l[1]/q[1], -.5*l[2]/q[2] };
            return (
                hasDiagonalArtifactInner(artifactClassifier, am, dm, a, l, q, a[1]-a[0], b[1]-b[0]+c[1]-c[0], d[1]-d[0], tEx[0], tEx[1]) ||
                hasDiagonalArtifactInner(artifactClassifier, am, dm, a, l, q, a[2]-a[1], b[2]-b[1]+c[2]-c[1], d[2]-d[1], tEx[1], tEx[2]) ||
                hasDiagonalArtifactInner(artifactClassifier, am, dm, a, l, q, a[0]-a[2], b[0]-b[2]+c[0]-c[2], d[0]-d[2], tEx[2], tEx[0])
            );
        }
        return false;
    }

    // ---- ErrorCorrector ----

    ErrorCorrector::ErrorCorrector() { }

    ErrorCorrector::ErrorCorrector(const Image<uint8_t, 1>& stencil, const Transformation& transformation) : stencil{stencil}, transformation{transformation} {
        memset((void*)stencil.pixels(), 0, stencil.width() * stencil.height());
    }

    void ErrorCorrector::setMinDeviationRatio(double r) {
        minDeviationRatio = r;
    }
    void ErrorCorrector::setMinImproveRatio(double r) {
        minImproveRatio = r;
    }

    void ErrorCorrector::protectCorners(const Shape& shape) {
        for (const Contour& contour : shape.contours) {
            if (!contour.edges.empty()) {
                const Segment* prevEdge = contour.edges.back().get();
                for (const Edge& edge : contour.edges) {
                    int shared = sharedChannelCount(prevEdge->color, edge.get()->color);
                    if (shared <= 1) {
                        vec2 p = transformation.project(edge.get()->point(0));
                        int l = (int)floor(p.x - 0.5);
                        int b = (int)floor(p.y - 0.5);
                        if (shape.inverseYAxis) {
                            b = stencil.height() - b - 2;
                        }
                        int r = l + 1;
                        int t = b + 1;
                        if (l < stencil.width() && b < stencil.height() && r >= 0 && t >= 0) {
                            if (l >= 0 && b >= 0) *stencil(l, b) |= (uint8_t)PROTECTED;
                            if (r < stencil.width() && b >= 0) *stencil(r, b) |= (uint8_t)PROTECTED;
                            if (l >= 0 && t < stencil.height()) *stencil(l, t) |= (uint8_t)PROTECTED;
                            if (r < stencil.width() && t < stencil.height()) *stencil(r, t) |= (uint8_t)PROTECTED;
                        }
                    }
                    prevEdge = edge.get();
                }
            }
        }
    }

    void ErrorCorrector::protectEdges(const Image<float, 3>& sdf) {
        constexpr int N = 3;
        float radius;
        // Horizontal texel pairs
        radius = float(PROTECTION_RADIUS_TOLERANCE * qbn::length(transformation.unproject(vec2(transformation.distanceMapping(DistanceMapping::Delta(1)), 0))));
        for (int y = 0; y < sdf.height(); ++y) {
            const float* left = sdf(0, y);
            const float* right = sdf(1, y);
            for (int x = 0; x < sdf.width() - 1; ++x) {
                float lm = median(left[0], left[1], left[2]);
                float rm = median(right[0], right[1], right[2]);
                if (fabsf(lm - 0.5f) + fabsf(rm - 0.5f) < radius) {
                    Color mask = edgeBetweenTexels(left, right);
                    protectExtremeChannels(stencil(x, y), left, lm, mask);
                    protectExtremeChannels(stencil(x + 1, y), right, rm, mask);
                }
                left += N, right += N;
            }
        }
        // Vertical texel pairs
        radius = float(PROTECTION_RADIUS_TOLERANCE * qbn::length(transformation.unproject(vec2(0, transformation.distanceMapping(DistanceMapping::Delta(1))))));
        for (int y = 0; y < sdf.height() - 1; ++y) {
            const float* bottom = sdf(0, y);
            const float* top = sdf(0, y + 1);
            for (int x = 0; x < sdf.width(); ++x) {
                float bm = median(bottom[0], bottom[1], bottom[2]);
                float tm = median(top[0], top[1], top[2]);
                if (fabsf(bm - 0.5f) + fabsf(tm - 0.5f) < radius) {
                    Color mask = edgeBetweenTexels(bottom, top);
                    protectExtremeChannels(stencil(x, y), bottom, bm, mask);
                    protectExtremeChannels(stencil(x, y + 1), top, tm, mask);
                }
                bottom += N, top += N;
            }
        }
        // Diagonal texel pairs
        radius = float(PROTECTION_RADIUS_TOLERANCE * qbn::length(transformation.unproject(vec2(transformation.distanceMapping(DistanceMapping::Delta(1))))));
        for (int y = 0; y < sdf.height() - 1; ++y) {
            const float* lb = sdf(0, y);
            const float* rb = sdf(1, y);
            const float* lt = sdf(0, y + 1);
            const float* rt = sdf(1, y + 1);
            for (int x = 0; x < sdf.width() - 1; ++x) {
                float mlb = median(lb[0], lb[1], lb[2]);
                float mrb = median(rb[0], rb[1], rb[2]);
                float mlt = median(lt[0], lt[1], lt[2]);
                float mrt = median(rt[0], rt[1], rt[2]);
                if (fabsf(mlb - 0.5f) + fabsf(mrt - 0.5f) < radius) {
                    Color mask = edgeBetweenTexels(lb, rt);
                    protectExtremeChannels(stencil(x, y), lb, mlb, mask);
                    protectExtremeChannels(stencil(x + 1, y + 1), rt, mrt, mask);
                }
                if (fabsf(mrb - 0.5f) + fabsf(mlt - 0.5f) < radius) {
                    Color mask = edgeBetweenTexels(rb, lt);
                    protectExtremeChannels(stencil(x + 1, y), rb, mrb, mask);
                    protectExtremeChannels(stencil(x, y + 1), lt, mlt, mask);
                }
                lb += N, rb += N, lt += N, rt += N;
            }
        }
    }

    void ErrorCorrector::protectAll() {
        uint8_t* end = stencil.pixels() + stencil.width() * stencil.height();
        for (uint8_t* mask = stencil.pixels(); mask < end; ++mask) {
            *mask |= (uint8_t)PROTECTED;
        }
    }

    void ErrorCorrector::find(const Image<float, 3>& sdf) {
        double hSpan = minDeviationRatio * qbn::length(transformation.unproject(vec2(transformation.distanceMapping(DistanceMapping::Delta(1)), 0)));
        double vSpan = minDeviationRatio * qbn::length(transformation.unproject(vec2(0, transformation.distanceMapping(DistanceMapping::Delta(1)))));
        double dSpan = minDeviationRatio * qbn::length(transformation.unproject(vec2(transformation.distanceMapping(DistanceMapping::Delta(1)))));
        for (int y = 0; y < sdf.height(); ++y) {
            for (int x = 0; x < sdf.width(); ++x) {
                const float* c = sdf(x, y);
                float cm = median(c[0], c[1], c[2]);
                bool protectedFlag = (*stencil(x, y) & PROTECTED) != 0;
                const float* l = NULL;
                const float* b = NULL;
                const float* r = NULL;
                const float* t = NULL;
                *stencil(x, y) |= (uint8_t) (ERROR * (
                    (x > 0 && ((l = sdf(x - 1, y)), hasLinearArtifact(BaseArtifactClassifier(hSpan, protectedFlag), cm, c, l))) ||
                    (y > 0 && ((b = sdf(x, y - 1)), hasLinearArtifact(BaseArtifactClassifier(vSpan, protectedFlag), cm, c, b))) ||
                    (x < sdf.width() - 1 && ((r = sdf(x + 1, y)), hasLinearArtifact(BaseArtifactClassifier(hSpan, protectedFlag), cm, c, r))) ||
                    (y < sdf.height() - 1 && ((t = sdf(x, y + 1)), hasLinearArtifact(BaseArtifactClassifier(vSpan, protectedFlag), cm, c, t))) ||
                    (x > 0 && y > 0 && hasDiagonalArtifact(BaseArtifactClassifier(dSpan, protectedFlag), cm, c, l, b, sdf(x - 1, y - 1))) ||
                    (x < sdf.width() - 1 && y > 0 && hasDiagonalArtifact(BaseArtifactClassifier(dSpan, protectedFlag), cm, c, r, b, sdf(x + 1, y - 1))) ||
                    (x > 0 && y < sdf.height() - 1 && hasDiagonalArtifact(BaseArtifactClassifier(dSpan, protectedFlag), cm, c, l, t, sdf(x - 1, y + 1))) ||
                    (x < sdf.width() - 1 && y < sdf.height() - 1 && hasDiagonalArtifact(BaseArtifactClassifier(dSpan, protectedFlag), cm, c, r, t, sdf(x + 1, y + 1)))
                ));
            }
        }
    }

    // Internal, non-template dispatch to the two possible combiners.
    template <class Combiner>
    static void findErrorsWithCombiner(const Image<float, 3>& sdf, const Shape& shape, const Transformation& transformation, double minDeviationRatio, double minImproveRatio, Image<uint8_t, 1>& stencil, ErrorCorrector::Flags ERROR, ErrorCorrector::Flags PROTECTED) {
        double hSpan = minDeviationRatio * qbn::length(transformation.unproject(vec2(transformation.distanceMapping(DistanceMapping::Delta(1)), 0)));
        double vSpan = minDeviationRatio * qbn::length(transformation.unproject(vec2(0, transformation.distanceMapping(DistanceMapping::Delta(1)))));
        double dSpan = minDeviationRatio * qbn::length(transformation.unproject(vec2(transformation.distanceMapping(DistanceMapping::Delta(1)))));

        ShapeDistanceChecker<Combiner> checker(sdf, shape, transformation, transformation.distanceMapping, minImproveRatio);
        bool rightToLeft = false;
        for (int y = 0; y < sdf.height(); ++y) {
            int row = shape.inverseYAxis ? sdf.height() - y - 1 : y;
            for (int col = 0; col < sdf.width(); ++col) {
                int x = rightToLeft ? sdf.width() - col - 1 : col;
                if ((*stencil(x, row) & ERROR)) {
                    continue;
                }
                const float* c = sdf(x, row);
                checker.shapeCoord = transformation.unproject(vec2(x + 0.5, y + 0.5));
                checker.sdfCoord = vec2(x + 0.5, row + 0.5);
                checker.msd = c;
                checker.protectedFlag = (*stencil(x, row) & PROTECTED) != 0;
                float cm = median(c[0], c[1], c[2]);
                const float* l = NULL;
                const float* b = NULL;
                const float* r = NULL;
                const float* t = NULL;
                *stencil(x, row) |= (uint8_t) (ERROR * (
                    (x > 0 && ((l = sdf(x - 1, row)), hasLinearArtifact(checker.classifier(vec2(-1.0, 0.0), hSpan), cm, c, l))) ||
                    (row > 0 && ((b = sdf(x, row - 1)), hasLinearArtifact(checker.classifier(vec2(0.0, -1.0), vSpan), cm, c, b))) ||
                    (x < sdf.width() - 1 && ((r = sdf(x + 1, row)), hasLinearArtifact(checker.classifier(vec2(1.0, 0.0), hSpan), cm, c, r))) ||
                    (row < sdf.height() - 1 && ((t = sdf(x, row + 1)), hasLinearArtifact(checker.classifier(vec2(0.0, 1.0), vSpan), cm, c, t))) ||
                    (x > 0 && row > 0 && hasDiagonalArtifact(checker.classifier(vec2(-1.0, -1.0), dSpan), cm, c, l, b, sdf(x - 1, row - 1))) ||
                    (x < sdf.width() - 1 && row > 0 && hasDiagonalArtifact(checker.classifier(vec2(1.0, -1.0), dSpan), cm, c, r, b, sdf(x + 1, row - 1))) ||
                    (x > 0 && row < sdf.height() - 1 && hasDiagonalArtifact(checker.classifier(vec2(-1.0, 1.0), dSpan), cm, c, l, t, sdf(x - 1, row + 1))) ||
                    (x < sdf.width() - 1 && row < sdf.height() - 1 && hasDiagonalArtifact(checker.classifier(vec2(1.0, 1.0), dSpan), cm, c, r, t, sdf(x + 1, row + 1)))
                ));
            }
        }
    }

    void ErrorCorrector::find(const Image<float, 3>& sdf, const Shape& shape, bool overlapSupport) {
        if (overlapSupport) {
            findErrorsWithCombiner<OverlappingContourCombiner>(sdf, shape, transformation, minDeviationRatio, minImproveRatio, stencil, ERROR, PROTECTED);
        } else {
            findErrorsWithCombiner<ContourCombiner>(sdf, shape, transformation, minDeviationRatio, minImproveRatio, stencil, ERROR, PROTECTED);
        }
    }

    void ErrorCorrector::apply(Image<float, 3>& sdf) const {
        constexpr int N = 3;
        int texelCount = sdf.width() * sdf.height();
        const uint8_t* mask = stencil.pixels();
        float* texel = sdf.pixels();
        for (int i = 0; i < texelCount; ++i) {
            if (*mask & ERROR) {
                float m = median(texel[0], texel[1], texel[2]);
                texel[0] = m, texel[1] = m, texel[2] = m;
            }
            ++mask;
            texel += N;
        }
    }

    Image<uint8_t, 1> ErrorCorrector::getStencil() const {
        return stencil;
    }

    // ---- Top-level driver ----

    static void errorCorrectionInner(Image<float, 3>& sdf, const Shape& shape, const Transformation& transformation, const MSDFConfig& config) {
        if (config.errorCorrection.mode == ErrorCorrectionConfig::DISABLED) {
            return;
        }

        Image<uint8_t, 1> stencilBuffer;
        if (!config.errorCorrection.buffer) {
            stencilBuffer = Image<uint8_t, 1>(sdf.width(), sdf.height());
        }

        Image<uint8_t, 1> stencil(
            config.errorCorrection.buffer ? config.errorCorrection.buffer : stencilBuffer.pixels(),
            sdf.width(), sdf.height()
        );

        ErrorCorrector ec(stencil, transformation);
        ec.setMinDeviationRatio(config.errorCorrection.minDeviationRatio);
        ec.setMinImproveRatio(config.errorCorrection.minImproveRatio);

        switch (config.errorCorrection.mode) {
            case ErrorCorrectionConfig::DISABLED:
            case ErrorCorrectionConfig::INDISCRIMINATE:
                break;
            case ErrorCorrectionConfig::EDGE_PRIORITY:
                ec.protectCorners(shape);
                ec.protectEdges(sdf);
                break;
            case ErrorCorrectionConfig::EDGE_ONLY:
                ec.protectAll();
                break;
        }

        if (config.errorCorrection.distanceCheckMode == ErrorCorrectionConfig::DO_NOT_CHECK_DISTANCE ||
            (config.errorCorrection.distanceCheckMode == ErrorCorrectionConfig::CHECK_DISTANCE_AT_EDGE && config.errorCorrection.mode != ErrorCorrectionConfig::EDGE_ONLY)) {
            ec.find(sdf);
            if (config.errorCorrection.distanceCheckMode == ErrorCorrectionConfig::CHECK_DISTANCE_AT_EDGE) {
                ec.protectAll();
            }
        }

        if (config.errorCorrection.distanceCheckMode == ErrorCorrectionConfig::ALWAYS_CHECK_DISTANCE ||
            config.errorCorrection.distanceCheckMode == ErrorCorrectionConfig::CHECK_DISTANCE_AT_EDGE) {
            ec.find(sdf, shape, config.overlapSupport);
        }

        ec.apply(sdf);
    }

    static void errorCorrectionShapeless(Image<float, 3>& sdf, const Transformation& transformation, double minDeviationRatio, bool protectAll) {
        Image<uint8_t, 1> stencilBuffer(sdf.width(), sdf.height());
        ErrorCorrector ec(stencilBuffer, transformation);
        ec.setMinDeviationRatio(minDeviationRatio);
        if (protectAll) {
            ec.protectAll();
        }
        ec.find(sdf);
        ec.apply(sdf);
    }

    void errorCorrection(Image<float, 3>& sdf, const Shape& shape, const Transformation& transformation, const MSDFConfig& config) {
        errorCorrectionInner(sdf, shape, transformation, config);
    }
    void errorCorrection(Image<float, 3>& sdf, const Shape& shape, const Projection& projection, Range range, const MSDFConfig& config) {
        errorCorrectionInner(sdf, shape, Transformation(projection, range), config);
    }

    void fastDistanceErrorCorrection( Image<float, 3>& sdf, const Transformation& transformation, double minDeviationRatio) {
        errorCorrectionShapeless(sdf, transformation, minDeviationRatio, false);
    }
    void fastDistanceErrorCorrection(Image<float, 3>& sdf, const Projection& projection, Range range, double minDeviationRatio) {
        errorCorrectionShapeless(sdf, Transformation(projection, range), minDeviationRatio, false);
    }
    void fastDistanceErrorCorrection(Image<float, 3>& sdf, Range pxRange, double minDeviationRatio) {
        errorCorrectionShapeless(sdf, Transformation(Projection(), pxRange), minDeviationRatio, false);
    }

    void fastEdgeErrorCorrection(Image<float, 3>& sdf, const Transformation& transformation, double minDeviationRatio) {
        errorCorrectionShapeless(sdf, transformation, minDeviationRatio, true);
    }
    void fastEdgeErrorCorrection(Image<float, 3>& sdf, const Projection& projection, Range range, double minDeviationRatio) {
        errorCorrectionShapeless(sdf, Transformation(Projection(), range), minDeviationRatio, true);
    }
    void fastEdgeErrorCorrection(Image<float, 3>& sdf, Range pxRange, double minDeviationRatio) {
        errorCorrectionShapeless(sdf, Transformation(Projection(), pxRange), minDeviationRatio, true);
    }
}