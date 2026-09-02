#ifndef __QONTEXT_MSDF_HPP__
#define __QONTEXT_MSDF_HPP__

// std
#include <string>

// msdf
#include <msdf/msdf_atlas.hpp>
#include <msdf/atlas/charset.hpp>
#include <msdf/atlas/atlas_config.hpp>

namespace qtxt {
    class MSDF {
        public:
            static MSDFAtlas load(const std::string& fontPath, Charset charset = Charset::ASCII, AtlasConfig config = {}, std::string atlasPath = "");
        private:
    };
}

#endif