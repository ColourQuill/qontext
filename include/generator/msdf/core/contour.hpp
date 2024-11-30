#ifndef __QONTEXT_CONTOUR_HPP__
#define __QONTEXT_CONTOUR_HPP__

// std
#include <vector>

// msdf
#include <generator/msdf/core/edge_holder.hpp>

namespace qtxt {
    class Contour {
        public:
            std::vector<EdgeHolder> edges;

            void AddEdge(const EdgeHolder& edge);

            EdgeHolder& AddEdge();
            
            void Bound(double& l, double& b, double& r, double& t) const;
            void BoundMiters(double& l, double& b, double& r, double& t, double border, double miterLimit, int polarity) const;
            int Winding() const;
            void Reverse();
    };
};

#endif