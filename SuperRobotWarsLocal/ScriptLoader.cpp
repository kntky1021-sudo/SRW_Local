#include "ScriptLoader.h"
#include <fstream>
#include <iostream>

nlohmann::json ScriptLoader::load(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open script file: " << path << "\n";
        return {};
    }

    try {
        nlohmann::json json;
        file >> json;
        return json;
    }
    catch (const std::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << "\n";
        return {};
    }
}