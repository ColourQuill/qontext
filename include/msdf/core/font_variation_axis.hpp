#ifndef __QONTEXT_FONT_VARIATION_AXIS_HPP__
#define __QONTEXT_FONT_VARIATION_AXIS_HPP__

namespace qtxt {
    struct FontVariationAxis {
        const char* name;
        double minValue;
        double maxValue;
        double defaultValue;
    };
}

#endif