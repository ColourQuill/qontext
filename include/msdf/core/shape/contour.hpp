#ifndef __QONTEXT_CONTOUR_HPP__
#define __QONTEXT_CONTOUR_HPP__

// std
#include <vector>

// msdf
#include <msdf/core/shape/edge.hpp>

namespace qtxt {
    class Contour {
        public:
            std::vector<Edge> edges;

            void add(const Edge& edge);

            Edge& add();
            
            void bound(double& l, double& b, double& r, double& t) const;
            void boundMiters(double& l, double& b, double& r, double& t, double border, double miterLimit, int polarity) const;
            int winding() const;
            void reverse();
    };
};

#endif