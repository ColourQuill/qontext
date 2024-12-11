#ifndef __QONTEXT_CONFIG_WRITER_HPP__
#define __QONTEXT_CONFIG_WRITER_HPP__

// file types
#include <file_types/config.hpp>

// writer
#include <writer/writer.hpp>

namespace qtxt {
    template<> struct Writer<Config> : public Writer<TEXT> {
        public:
            void OverwriteConfig(const std::string& path, Config config);
            void OverwriteConfig(const char* path, Config config);
        private:

    };
}

#endif