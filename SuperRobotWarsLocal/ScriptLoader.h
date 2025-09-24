#pragma once
#include <string>
#include <nlohmann/json.hpp>

class ScriptLoader {
public:
    // ファイルが開けない／パースに失敗した場合は空の JSON を返す
    static nlohmann::json load(const std::string& filepath);
};