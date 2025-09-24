#pragma once
#include <string>
#include <vector>

class BattleMap {
public:
    // JSON ファイル (maps/{mapId}.json) を読み込む
    bool load(const std::string& mapId);

    // 読み込んだマップをコンソールに描画
    void printConsole() const;

    int getWidth() const;
    int getHeight() const;
    int getTile(int x, int y) const;

private:
    int width = 0;
    int height = 0;
    std::vector<std::vector<int>> tiles;
};