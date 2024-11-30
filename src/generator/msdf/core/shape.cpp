#include <generator/msdf/core/shape.hpp>

#define DECONVERGE_OVERSHOOT 1.11111111111111111

// std
#include <cstdlib>
#include <iostream>

// msdf
#include <generator/msdf/core/arithmetic.hpp>
#include <generator/msdf/core/bounds.hpp>
#include <generator/msdf/core/scanline.hpp>
#include <generator/msdf/core/edge_holder.hpp>

namespace qtxt {
    Shape::Shape() : inverseYAxis{false} {}

    void Shape::AddContour(const Contour& contour) {
        contours.push_back(contour);
    }
    Contour& Shape::AddContour() {
        contours.resize(contours.size() + 1);
        return contours.back();
    }

    static void DeconvergeEdge(EdgeHolder& edgeHolder, int param, vec2 vector) {
        switch (edgeHolder->Type()) {
            case (int) QuadraticSegment::EDGE_TYPE:
                edgeHolder = static_cast<const QuadraticSegment*>(&*edgeHolder)->ConvertToCubic();
                // fallthrough
            case (int) CubicSegment::EDGE_TYPE:
                {
                    vec2* p = static_cast<CubicSegment*>(&*edgeHolder)->p;
                    switch (param) {
                        case 0:
                            p[1] += qbn::length(p[1] - p[0]) * vector;
                            break;
                        case 1:
                            p[2] += qbn::length(p[2] - p[3]) * vector;
                            break;
                    }
                }
        }
    }

    void Shape::Normalize() {
        for (Contour& contour : contours) {
            if (contour.edges.size() == 1) {
                EdgeSegment* parts[3] = {};
                contour.edges[0]->SplitInThirds(parts[0], parts[1], parts[2]);
                contour.edges.clear();
                contour.edges.push_back(EdgeHolder(parts[0]));
                contour.edges.push_back(EdgeHolder(parts[1]));
                contour.edges.push_back(EdgeHolder(parts[2]));
            } else {
                EdgeHolder* prevEdge = &contour.edges.back();
                for (EdgeHolder& edge : contour.edges) {
                    vec2 prevDir = qbn::norm((*prevEdge)->Direction(1));
                    vec2 curDir = qbn::norm(edge->Direction(0));
                    if (qbn::dot(prevDir, curDir) < MSDF_CORNER_DOT_EPSILON - 1) {
                        double factor = DECONVERGE_OVERSHOOT * sqrt(1 - (MSDF_CORNER_DOT_EPSILON - 1) * (MSDF_CORNER_DOT_EPSILON-1)) / (MSDF_CORNER_DOT_EPSILON - 1);
                        vec2 axis = factor * qbn::norm(curDir - prevDir);
                        if (qbn::cross((*prevEdge)->DirectionChange(1), edge->Direction(0)) + qbn::cross(edge->DirectionChange(0), (*prevEdge)->Direction(1)) < 0) {
                            axis = -axis;
                        }
                        DeconvergeEdge(*prevEdge, 1, qbn::orthogonal2D(axis, true));
                        DeconvergeEdge(edge, 0, qbn::orthogonal2D(axis, false));
                    }
                    prevEdge = &edge;
                }
            }
        }
    }
    bool Shape::Validate() const {
        for (const Contour& contour : contours) {
            if (!contour.edges.empty()) {
                vec2 corner = contour.edges.back()->Point(1);
                for (const EdgeHolder& edge : contour.edges) {
                    if (!edge) {
                        return false;
                    }
                    if (edge->Point(0) != corner) {
                        return false;
                    }
                    corner = edge->Point(1);
                }
            }
        }
        return true;
    }
    void Shape::Bound(double& l, double& b, double& r, double& t) const {
        for (const Contour& contour : contours) {
            contour.Bound(l, b, r, t);
        }
    }
    void Shape::BoundMiters(double& l, double& b, double& r, double& t, double border, double miterLimit, int polarity) const {
        for (const Contour& contour : contours) {
            contour.BoundMiters(l, b, r, t, border, miterLimit, polarity);
        }
    }
    Bounds Shape::GetBounds(double border, double miterLimit, int polarity) const {
        static const double LARGE_VALUE = 1e240;
        Bounds bounds = { +LARGE_VALUE, +LARGE_VALUE, -LARGE_VALUE, -LARGE_VALUE };
        Bound(bounds.l, bounds.b, bounds.r, bounds.t);
        if (border > 0) {
            bounds.l -= border, bounds.b -= border;
            bounds.r += border, bounds.t += border;
            if (miterLimit > 0) {
                BoundMiters(bounds.l, bounds.b, bounds.r, bounds.t, border, miterLimit, polarity);
            }
        }
        return bounds;
    }
    void Shape::DoScanline(Scanline& line, double y) const {
        std::vector<Intersection> intersections;
        double x[3];
        int dy[3];
        for (const Contour& contour : contours) {
            for (const EdgeHolder& edge : contour.edges) {
                int n = edge->ScanlineIntersections(x, dy, y);
                for (int i = 0; i < n; ++i) {
                    Intersection intersection = {x[i], dy[i]};
                    intersections.push_back(intersection);
                }
            }
        }
        line.SetIntersections(intersections.data(), intersections.size());
    }
    int Shape::EdgeCount() const {
        int total = 0;
        for (const Contour& contour : contours) {
            total += (int)contour.edges.size();
        }
        return total;
    }
    void Shape::OrientContours() {
        struct Intersection {
            double x;
            int direction;
            int contourIndex;

            static int Compare(const void* a, const void* b) {
                return sign(reinterpret_cast<const Intersection*>(a)->x - reinterpret_cast<const Intersection*>(b)->x);
            }
        };

        const double ratio = 0.5 * (sqrt(5) - 1); // an irrational number to minimize chance of intersecting a corner or other point of interest
        std::vector<int> orientations(contours.size());
        std::vector<Intersection> intersections;
        for (int i = 0; i < (int)contours.size(); ++i) {
            if (!orientations[i] && !contours[i].edges.empty()) {
                // Find an Y that crosses the contour
                double y0 = contours[i].edges.front()->Point(0).y;
                double y1 = y0;
                for (std::vector<EdgeHolder>::const_iterator edge = contours[i].edges.begin(); edge != contours[i].edges.end() && y0 == y1; ++edge) {
                    y1 = (*edge)->Point(1).y;
                }
                for (std::vector<EdgeHolder>::const_iterator edge = contours[i].edges.begin(); edge != contours[i].edges.end() && y0 == y1; ++edge) {
                    y1 = (*edge)->Point(ratio).y; // in case all endpoints are in a horizontal line
                }
                
                double y = mix(y0, y1, ratio);
                // Scanline through whole shape at Y
                double x[3];
                int dy[3];
                for (int j = 0; j < (int) contours.size(); ++j) {
                    for (const EdgeHolder& edge : contours[i].edges) {
                        int n = edge->ScanlineIntersections(x, dy, y);
                        for (int k = 0; k < n; ++k) {
                            Intersection intersection = {x[k], dy[k], j};
                            intersections.push_back(intersection);
                        }
                    }
                }
                if (!intersections.empty()) {
                    qsort(&intersections[0], intersections.size(), sizeof(Intersection), &Intersection::Compare);
                    // Disqualify multiple intersections
                    for (int j = 1; j < (int) intersections.size(); ++j) {
                        if (intersections[j].x == intersections[j-1].x) {
                            intersections[j].direction = intersections[j-1].direction = 0;
                        } 
                    }
                        
                    // Inspect scanline and deduce orientations of intersected contours
                    for (int j = 0; j < (int) intersections.size(); ++j) {
                        if (intersections[j].direction) {
                            orientations[intersections[j].contourIndex] += 2*((j&1)^(intersections[j].direction > 0))-1;
                        }
                    }   
                    intersections.clear();
                }
            }
        }
        // Reverse contours that have the opposite orientation
        for (int i = 0; i < (int) contours.size(); ++i) {
            if (orientations[i] < 0) {
                contours[i].Reverse();
            }
        }
    }
}