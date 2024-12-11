#include <reader/config_reader.hpp>

// std
#include <stdexcept>

namespace qtxt {
    Config Reader<Config>::GetConfigFromFile(const std::string& path) {
        return GetConfigFromFile(path.c_str());
    }
    Config Reader<Config>::GetConfigFromFile(const char* path) {
        ReadFile(path);
        Config config;
        Parse(config);
        ClearBuffer();
        return config;
    }

    std::string Reader<Config>::Trim(const std::string& str) const {
        size_t first = str.find_first_not_of(" \t");
        size_t last = str.find_last_not_of(" \t");
        return (first == std::string::npos) ? "" : str.substr(first, last - first + 1);
    }

    void Reader<Config>::Parse(Config& config) {
        std::string currentSection;
        for (unsigned long long i = 0; i < buffer.size(); ++i) {
            std::string line = Trim(buffer[i]);

            if (line.empty() || line[0] == '#' || line[0] == ';') {
                continue;
            }

            if (line[0] == '[' && line.back() == ']') {
                currentSection = line.substr(1, line.size() - 2);
                config.data[currentSection] = {};
            } else {
                size_t delimiterPos = line.find('=');
                if (delimiterPos != std::string::npos) {
                    std::string key = Trim(line.substr(0, delimiterPos));
                    std::string value = Trim(line.substr(delimiterPos + 1));
                    config.data[currentSection].data[key] = value;
                } else {
                    throw std::runtime_error("Invalid line: " + line);
                }
            }
        }
    }
}