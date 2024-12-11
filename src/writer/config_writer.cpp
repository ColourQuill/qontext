#include <writer/config_writer.hpp>

namespace qtxt {
    void Writer<Config>::OverwriteConfig(const std::string& path, Config config) {
        OverwriteConfig(path.c_str(), config);
    }
    void Writer<Config>::OverwriteConfig(const char* path, Config config) {
        ClearBuffer();
        for (auto& sectionKV : config.data) {
            std::string section = "[" + sectionKV.first + "]";
            WriteLine(section);

            int i = 0;
            int size = sectionKV.second.data.size();
            for (auto& kv : sectionKV.second.data) {
                std::string key_value = kv.first + " = " + kv.second;
                WriteLine(key_value);
                ++i;
                if (i >= size) {
                    WriteLine("");
                }
            }
        }
        SaveFile(path);
        ClearBuffer();
    }
}