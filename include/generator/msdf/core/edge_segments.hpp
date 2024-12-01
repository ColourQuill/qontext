#ifndef __QONTEXT_EDGE_SEGMENTS_HPP__
#define __QONTEXT_EDGE_SEGMENTS_HPP__

// qarbon
#include <qarbon.hpp>

// msdf
#include <generator/msdf/core/signed_distance.hpp>
#include <generator/msdf/core/edge_color.hpp>

#define MSDF_CUBIC_SEARCH_STARTS 4
#define MSDF_CUBIC_SEARCH_STEPS 4

namespace qtxt {
    using vec2 = qbn::vec2<double>;

    class EdgeSegment {
        public:
            EdgeColor color;

            static EdgeSegment* Create(vec2 p0, vec2 p1, EdgeColor edgeColor = WHITE);
            static EdgeSegment* Create(vec2 p0, vec2 p1, vec2 p2, EdgeColor edgeColor = WHITE);
            static EdgeSegment* Create(vec2 p0, vec2 p1, vec2 p2, vec2 p3, EdgeColor edgeColor = WHITE);

            EdgeSegment(EdgeColor edgeColor = WHITE) : color{edgeColor} {}
            virtual ~EdgeSegment() { }
            // Creates a copy of the edge segment
            virtual EdgeSegment* Clone() const = 0;
            // Returns the numeric code of the edge segment's type
            virtual int Type() const = 0;
            // Returns the array of control points
            virtual const vec2* ControlPoints() const = 0;
            // Returns the point on the edge specified by the parameter (between 0 and 1)
            virtual vec2 Point(double param) const = 0;
            // Returns the direction the edge has at the point specified by the parameter
            virtual vec2 Direction(double param) const = 0;
            // Returns the change of direction (second derivative) at the point specified by the parameter
            virtual vec2 DirectionChange(double param) const = 0;
            // Returns the minimum signed distance between origin and the edge
            virtual SignedDistance GetSignedDistance(vec2 origin, double& param) const = 0;
            // Converts a previously retrieved signed distance from origin to perpendicular distance
            virtual void DistanceToPerpendicularDistance(SignedDistance& distance, vec2 origin, double param) const;
            // Outputs a list of (at most three) intersections (their X coordinates) with an infinite horizontal scanline at y and returns how many there are
            virtual int ScanlineIntersections(double x[3], int dy[3], double y) const = 0;
            // Adjusts the bounding box to fit the edge segment
            virtual void Bound(double& l, double& b, double& r, double& t) const = 0;

            // Reverses the edge (swaps its start point and end point)
            virtual void Reverse() = 0;
            // Moves the start point of the edge segment
            virtual void MoveStartPoint(vec2 to) = 0;
            // Moves the end point of the edge segment
            virtual void MoveEndPoint(vec2 to) = 0;
            // Splits the edge segments into thirds which together represent the original edge
            virtual void SplitInThirds(EdgeSegment*& part0, EdgeSegment*& part1, EdgeSegment*& part2) const = 0;
    };

    // A line segment
    class LinearSegment : public EdgeSegment {
        public:
            enum EdgeType {
                EDGE_TYPE = 1
            };

            vec2 p[2];

            LinearSegment(vec2 p0, vec2 p1, EdgeColor edgeColor = WHITE);
            LinearSegment* Clone() const;
            int Type() const;
            const vec2* ControlPoints() const;
            vec2 Point(double param) const;
            vec2 Direction(double param) const;
            vec2 DirectionChange(double param) const;
            double Length() const;
            SignedDistance GetSignedDistance(vec2 origin, double& param) const;
            int ScanlineIntersections(double x[3], int dy[3], double y) const;
            void Bound(double& l, double& b, double& r, double& t) const;

            void Reverse();
            void MoveStartPoint(vec2 to);
            void MoveEndPoint(vec2 to);
            void SplitInThirds(EdgeSegment*& part0, EdgeSegment*& part1, EdgeSegment*& part2) const;
    };

    class QuadraticSegment : public EdgeSegment {
        public:
            enum EdgeType {
                EDGE_TYPE = 2
            };

            vec2 p[3];

            QuadraticSegment(vec2 p0, vec2 p1, vec2 p2, EdgeColor edgeColor = WHITE);
            QuadraticSegment* Clone() const;
            int Type() const;
            const vec2* ControlPoints() const;
            vec2 Point(double param) const;
            vec2 Direction(double param) const;
            vec2 DirectionChange(double param) const;
            double Length() const;
            SignedDistance GetSignedDistance(vec2 origin, double& param) const;
            int ScanlineIntersections(double x[3], int dy[3], double y) const;
            void Bound(double& l, double& b, double& r, double& t) const;

            void Reverse();
            void MoveStartPoint(vec2 to);
            void MoveEndPoint(vec2 to);
            void SplitInThirds(EdgeSegment*& part0, EdgeSegment*& part1, EdgeSegment*& part2) const;

            EdgeSegment* ConvertToCubic() const;
    };

    class CubicSegment : public EdgeSegment {
        public:
            enum EdgeType {
                EDGE_TYPE = 3
            };

            vec2 p[4];

            CubicSegment(vec2 p0, vec2 p1, vec2 p2, vec2 p3, EdgeColor edgeColor = WHITE);
            CubicSegment* Clone() const;
            int Type() const;
            const vec2* ControlPoints() const;
            vec2 Point(double param) const;
            vec2 Direction(double param) const;
            vec2 DirectionChange(double param) const;
            SignedDistance GetSignedDistance(vec2 origin, double& param) const;
            int ScanlineIntersections(double x[3], int dy[3], double y) const;
            void Bound(double& l, double& b, double& r, double& t) const;

            void Reverse();
            void MoveStartPoint(vec2 to);
            void MoveEndPoint(vec2 to);
            void SplitInThirds(EdgeSegment*& part0, EdgeSegment*& part1, EdgeSegment*& part2) const;
    };
}

#endif