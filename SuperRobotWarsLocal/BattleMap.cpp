#include "BattleMap.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

bool BattleMap::load(const std::string& mapId) {
    std::string path = "maps/" + mapId + ".json";
    std::ifstream ifs(path);
    if (!ifs) {
        std::cerr << "BattleMap Error: failed to open " << path << "\n";
        return false;
    }

    nlohmann::json j;
    try {
        ifs >> j;
    }
    catch (const std::exception& e) {
        std::cerr << "BattleMap Error: JSON parse error: " << e.what() << "\n";
        return false;
    }

    if (!j.contains("width") || !j.contains("height") || !j.contains("tiles")) {
        std::cerr << "BattleMap Error: missing required fields in " << path << "\n";
        return false;
    }

    width = j["width"].get<int>();
    height = j["height"].get<int>();
    tiles.clear();
    tiles.resize(height, std::vector<int>(width));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            tiles[y][x] = j["tiles"][y][x].get<int>();
        }
    }
    return true;
}

void BattleMap::printConsole() const {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            char c = tiles[y][x] == 0 ? '.' : '#';
            std::cout << c;
        }
        std::cout << "\n";
    }
}

int BattleMap::getWidth() const { return width; }
int BattleMap::getHeight() const { return height; }
int BattleMap::getTile(int x, int y) const { return tiles[y][x]; }