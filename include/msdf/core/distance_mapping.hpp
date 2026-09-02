#ifndef __QONTEXT_DISTANCE_MAPPING_HPP__
#define __QONTEXT_DISTANCE_MAPPING_HPP__

namespace qtxt {
    // forward declaration
    struct Range;
    
    class DistanceMapping {
        public:
            class Delta {
                public:
                    double value;
                    inline explicit Delta(double distanceDelta) : value{distanceDelta} {}
                    inline operator double() const { return value; }
            };

            static DistanceMapping inverse(Range range);

            DistanceMapping();
            DistanceMapping(Range range);
            double operator()(double d) const;
            double operator()(Delta d) const;
            DistanceMapping inverse() const;
        private:
            double scale;
            double translate;

            inline DistanceMapping(double scale, double translate) : scale{scale}, translate{translate} {}
    };
}

#endif