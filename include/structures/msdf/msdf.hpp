#ifndef __MSDF_HPP__
#define __MSDF_HPP__

// std
#include <string>

// qtxt
#include <info/msdf_type.hpp>

#include <structures/font/font.hpp>

namespace qtxt {
    class MSDF {
        MSDF();

        static void Generate(MSDFType type, Font font);
    };
}

#endif