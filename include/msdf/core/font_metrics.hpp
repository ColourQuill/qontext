#ifndef __QONTEXT_FONT_METRICS_HPP__
#define __QONTEXT_FONT_METRICS_HPP__

namespace qtxt {
    struct FontMetrics {
        double emSize;
        double ascenderY;
        double descenderY;
        double lineHeight;
        double underlineY;
        double underlineThickness;
    };
}

#endif