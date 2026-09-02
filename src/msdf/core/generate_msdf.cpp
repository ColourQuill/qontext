#include <msdf/core/generate_msdf.hpp>

namespace qtxt {
    void generateMSDF(Image<float, 3>& output, const Shape& shape, const Transformation& transformation, const MSDFConfig& config) {
        ShapeDistanceFinder distanceFinder(shape, config.overlapSupport);

        bool rightToLeft = false;
        for (int y = 0; y < output.height(); ++y) {
            int row = shape.inverseYAxis ? output.height() - y - 1 : y;
            for (int col = 0; col < output.width(); ++col) {
                int x = rightToLeft ? output.width() - col - 1 : col;
                vec2 p = transformation.unproject(vec2(x + 0.5, y + 0.5));

                Distance d = distanceFinder.distance(p);

                float* px = output(x, row);
                px[0] = float(transformation.distanceMapping(d.r));
                px[1] = float(transformation.distanceMapping(d.g));
                px[2] = float(transformation.distanceMapping(d.b));
            }
            rightToLeft = !rightToLeft;
        }

        errorCorrection(output, shape, transformation, config);
    }
}