#include <msdf/core/shape/shape.hpp>

// std
#include <cstdlib>
#include <iostream>

// info
#include <info/properties.hpp>

// msdf
#include <msdf/core/arithmetic.hpp>
#include <msdf/core/bounds.hpp>
#include <msdf/core/scanline.hpp>
#include <msdf/core/shape/edge.hpp>
#include <msdf/core/shape/quadratic_segment.hpp>
#include <msdf/core/shape/cubic_segment.hpp>

namespace qtxt {
    static void deconvergeEdge(Edge& edge, int param, vec2 vector) {
        switch (edge.get()->type()) {
            case EdgeType::QUADRATIC:
                edge = static_cast<const QuadraticSegment*>(edge.get())->convert();
            case EdgeType::CUBIC:
                {
                    vec2* p = static_cast<CubicSegment*>(edge.get())->p;
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

    Shape::Shape() : inverseYAxis{false} {}

    void Shape::add(const Contour& contour) {
        contours.push_back(contour);
    }
    Contour& Shape::add() {
        contours.resize(contours.size() + 1);
        return contours.back();
    }

    void Shape::normalize() {
        for (Contour& contour : contours) {
            if (contour.edges.size() == 1) {
                Segment* segments[3] = {};
                contour.edges[0].get()->split(segments[0], segments[1], segments[2]);
                contour.edges.clear();
                contour.edges.push_back(Edge(segments[0]));
                contour.edges.push_back(Edge(segments[1]));
                contour.edges.push_back(Edge(segments[2]));
            } else {
                Edge* prevEdge = &contour.edges.back();
                for (Edge& edge : contour.edges) {
                    vec2 prevDir = qbn::norm(prevEdge->get()->direction(1));
                    vec2 curDir = qbn::norm(edge.get()->direction(0));
                    if (qbn::dot(prevDir, curDir) < MSDF_CORNER_DOT_EPSILON - 1) {
                        double factor = DECONVERGE_OVERSHOOT * sqrt(1 - (MSDF_CORNER_DOT_EPSILON - 1) * (MSDF_CORNER_DOT_EPSILON - 1)) / (MSDF_CORNER_DOT_EPSILON - 1);
                        vec2 axis = factor * qbn::norm(curDir - prevDir);
                        if (qbn::cross(prevEdge->get()->directionDelta(1), edge.get()->direction(0)) + qbn::cross(edge.get()->directionDelta(0), prevEdge->get()->direction(1)) < 0) {
                            axis = -axis;
                        }
                        deconvergeEdge(*prevEdge, 1, qbn::orthogonal2D(axis, true));
                        deconvergeEdge(edge, 0, qbn::orthogonal2D(axis, false));
                    }
                    prevEdge = &edge;
                }
            }
        }
    }

    bool Shape::validate() const {
        for (const Contour& contour : contours) {
            if (!contour.edges.empty()) {
                vec2 corner = contour.edges.back().get()->point(1);
                for (const Edge& edge : contour.edges) {
                    if (edge.empty()) {
                        return false;
                    }
                    if (edge.get()->point(0) != corner) {
                        return false;
                    }
                    corner = edge.get()->point(1);
                }
            }
        }
        return true;
    }

    void Shape::bound(double& l, double& b, double& r, double& t) const {
        for (const Contour& contour : contours) {
            contour.bound(l, b, r, t);
        }
    }

    void Shape::boundMiters(double& l, double& b, double& r, double& t, double border, double miterLimit, int polarity) const {
        for (const Contour& contour : contours) {
            contour.boundMiters(l, b, r, t, border, miterLimit, polarity);
        }
    }

    Bounds Shape::getBounds(double border, double miterLimit, int polarity) const {
        static const double LARGE_VALUE = 1e240;
        Bounds bounds = { +LARGE_VALUE, +LARGE_VALUE, -LARGE_VALUE, -LARGE_VALUE };
        bound(bounds.l, bounds.b, bounds.r, bounds.t);
        if (border > 0) {
            bounds.l -= border, bounds.b -= border;
            bounds.r += border, bounds.t += border;
            if (miterLimit > 0) {
                boundMiters(bounds.l, bounds.b, bounds.r, bounds.t, border, miterLimit, polarity);
            }
        }
        return bounds;
    }

    void Shape::scanline(Scanline& line, double y) const {
        std::vector<Intersection> intersections;
        double x[3];
        int dy[3];
        for (const Contour& contour : contours) {
            for (const Edge& edge : contour.edges) {
                int n = edge.get()->scanline(x, dy, y);
                for (int i = 0; i < n; ++i) {
                    Intersection intersection = {x[i], dy[i]};
                    intersections.push_back(intersection);
                }
            }
        }
        line.set(intersections.data(), intersections.size());
    }

    int Shape::edgeCount() const {
        int total = 0;
        for (const Contour& contour : contours) {
            total += (int)contour.edges.size();
        }
        return total;
    }

    void Shape::orient() {
        struct Intersection {
            double x;
            int direction;
            int contourIndex;

            static int compare(const void* a, const void* b) {
                return sign(reinterpret_cast<const Intersection*>(a)->x - reinterpret_cast<const Intersection*>(b)->x);
            }
        };

        const double ratio = 0.5 * (sqrt(5) - 1); // an irrational number to minimize chance of intersecting a corner or other point of interest
        std::vector<int> orientations(contours.size());
        std::vector<Intersection> intersections;
        for (int i = 0; i < (int)contours.size(); ++i) {
            if (!orientations[i] && !contours[i].edges.empty()) {
                // Find an Y that crosses the contour
                double y0 = contours[i].edges.front().get()->point(0).y;
                double y1 = y0;
                for (std::vector<Edge>::const_iterator edge = contours[i].edges.begin(); edge != contours[i].edges.end() && y0 == y1; ++edge) {
                    y1 = edge->get()->point(1).y;
                }
                for (std::vector<Edge>::const_iterator edge = contours[i].edges.begin(); edge != contours[i].edges.end() && y0 == y1; ++edge) {
                    y1 = edge->get()->point(ratio).y; // in case all endpoints are in a horizontal line
                }
                
                double y = mix(y0, y1, ratio);
                // Scanline through whole shape at Y
                double x[3];
                int dy[3];
                for (int j = 0; j < (int) contours.size(); ++j) {
                    for (const Edge& edge : contours[i].edges) {
                        int n = edge.get()->scanline(x, dy, y);
                        for (int k = 0; k < n; ++k) {
                            Intersection intersection = {x[k], dy[k], j};
                            intersections.push_back(intersection);
                        }
                    }
                }
                if (!intersections.empty()) {
                    qsort(&intersections[0], intersections.size(), sizeof(Intersection), &Intersection::compare);
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
                contours[i].reverse();
            }
        }
    }
}