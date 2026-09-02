#ifndef __QONTEXT_SEGMENT_HPP__
#define __QONTEXT_SEGMENT_HPP__

// msdf
#include <msdf/core/shape/signed_distance.hpp>
#include <msdf/core/shape/edge_type.hpp>
#include <msdf/core/shape/color.hpp>
#include <msdf/core/vec2.hpp>

namespace qtxt {
    class Segment {
        public:
            Color color;

            static Segment* create(vec2 p0, vec2 p1, Color color = Color::WHITE);
            static Segment* create(vec2 p0, vec2 p1, vec2 p2, Color color = Color::WHITE);
            static Segment* create(vec2 p0, vec2 p1, vec2 p2, vec2 p3, Color color = Color::WHITE);

            Segment(Color color = Color::WHITE) : color{color} {}
            virtual ~Segment() {}

            virtual Segment* clone() const = 0;
            virtual EdgeType type() const = 0;
            virtual const vec2* control() const = 0;
            virtual vec2 point(double param) const = 0;
            virtual vec2 direction(double param) const = 0;
            virtual vec2 directionDelta(double param) const = 0;
            virtual SignedDistance getSignedDistance(vec2 origin, double& param) const = 0;
            virtual void distanceToPerpendicularDistance(SignedDistance& distance, vec2 origin, double param) const;
            virtual int scanline(double x[3], int dy[3], double y) const = 0;
            virtual void bound(double& l, double& b, double& r, double& t) const = 0;

            virtual void reverse() = 0;
            virtual void moveStart(vec2 to) = 0;
            virtual void moveEnd(vec2 to) = 0;
            virtual void split(Segment*& segment0, Segment*& segment1, Segment*& segment2) const = 0;
        private:
    };
}

#endif