#include <iostream>

#include <filesystem>

int main() {
    std::filesystem::current_path(std::filesystem::canonical("/proc/self/exe").parent_path());

    

    return 0;
}
