#ifndef __QONTEXT_MSDF_CONFIG_HPP__
#define __QONTEXT_MSDF_CONFIG_HPP__

// msdf
#include <msdf/core/error_correction_config.hpp>

namespace qtxt {
    struct MSDFConfig {
        bool overlapSupport = true;
        ErrorCorrectionConfig errorCorrection = ErrorCorrectionConfig();
    };
}

#endif