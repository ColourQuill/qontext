#include <file_types/config.hpp>

namespace qtxt {
    std::string Config::GetValue(std::string header, std::string key) {
        return data[header].data[key];
    }
    int Config::GetIntValue(std::string header, std::string key) {
        return std::stoi(data[header].data[key]);
    }
    float Config::GetFloatValue(std::string header, std::string key) {
        return std::stof(data[header].data[key]);
    }
    double Config::GetDoubleValue(std::string header, std::string key) {
        return std::stod(data[header].data[key]);
    }
    bool Config::GetBoolValue(std::string header, std::string key) {
        std::string value = data[header].data[key];
        if (value == "true"    ||
            value == "True"    ||
            value == "yes"     ||
            value == "Yes"     ||
            value == "on"      ||
            value == "On"      || 
            value == "enable"  ||
            value == "Enable"  ||
            value == "enabled" ||
            value == "Enabled" ||
            value == "1"
        ) {
            return true;
        } else {
            return false;
        }
    }

    void Config::SetValue(std::string header, std::string key, std::string value) {
        data[header].data[key] = value;
    }
}