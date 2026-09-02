#include <msdf/core/shape/color.hpp>

namespace qtxt {
    bool hasRed(Color c) {
        return c == Color::RED || c == Color::YELLOW || c == Color::MAGENTA || c == Color::WHITE;
    }
    bool hasGreen(Color c) {
        return c == Color::GREEN || c == Color::YELLOW || c == Color::CYAN || c == Color::WHITE;
    }
    bool hasBlue(Color c) {
        return c == Color::BLUE || c == Color::MAGENTA || c == Color::CYAN || c == Color::WHITE;
    }
}