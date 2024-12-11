#ifndef __QONTEXT_CONFIG_HPP__
#define __QONTEXT_CONFIG_HPP__

// std
#include <map>
#include <string>

namespace qtxt {
    struct Config {
        public:
            std::string GetValue(std::string header, std::string key);
            int GetIntValue(std::string header, std::string key);
            float GetFloatValue(std::string header, std::string key);
            double GetDoubleValue(std::string header, std::string key);
            bool GetBoolValue(std::string header, std::string key);

            struct Section {
                std::map<std::string, std::string> data;
            };

            std::map<std::string, Section> data;

            void SetValue(std::string header, std::string key, std::string value);
    };
}

#endif