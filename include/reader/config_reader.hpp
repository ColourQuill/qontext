#ifndef __QONTEXT_CONFIG_READER_HPP__
#define __QONTEXT_CONFIG_READER_HPP__

// std
#include <map>
#include <string>

// filetypes
#include <file_types/config.hpp>

// reader
#include <reader/reader.hpp>

namespace qtxt {
    template<> struct Reader<Config> : public Reader<TEXT> {
        public:
            Config GetConfigFromFile(const std::string& path);
            Config GetConfigFromFile(const char* path);
        private:
            void Parse(Config& config);

            std::string Trim(const std::string& str) const;
    };
}

#endif