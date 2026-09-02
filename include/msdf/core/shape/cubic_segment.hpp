#ifndef __QONTEXT_CUBIC_SEGMENT_HPP__
#define __QONTEXT_CUBIC_SEGMENT_HPP__

// msdf
#include <msdf/core/shape/segment.hpp>

namespace qtxt {
    class CubicSegment : public Segment {
        public:
            vec2 p[4];

            CubicSegment(vec2 p0, vec2 p1, vec2 p2, vec2 p3, Color color = Color::WHITE);
            
            CubicSegment* clone() const;
            EdgeType type() const;
            const vec2* control() const;
            vec2 point(double param) const;
            vec2 direction(double param) const;
            vec2 directionDelta(double param) const;
            SignedDistance getSignedDistance(vec2 origin, double& param) const;
            int scanline(double x[3], int dy[3], double y) const;
            void bound(double& l, double& b, double& r, double& t) const;

            void reverse();
            void moveStart(vec2 to);
            void moveEnd(vec2 to);
            void split(Segment*& segment0, Segment*& segment1, Segment*& segment2) const;
        private:
    };
}

#endif