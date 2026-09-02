#ifndef __QONTEXT_COLOR_HPP__
#define __QONTEXT_COLOR_HPP__

namespace qtxt {
    enum class Color {
        BLACK,
        RED,
        GREEN,
        YELLOW,
        BLUE,
        MAGENTA,
        CYAN,
        WHITE
    };

    bool hasRed(Color c);
    bool hasGreen(Color c);
    bool hasBlue(Color c);
}

#endif