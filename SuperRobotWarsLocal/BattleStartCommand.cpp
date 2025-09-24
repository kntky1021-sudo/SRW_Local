#include "BattleStartCommand.h"
#include "ExecutionEngine.h"
#include "TileMap.h"
#include "BattleManager.h"
#include <iostream>

// 呼び出し側からの指定が "map01" / "map01.json" / "maps/map01" / "maps/map01.json"
// のいずれでも正しく単一のフルパスに正規化する
std::string BattleStartCommand::normalizeMapPath(const std::string& spec) {
    if (spec.empty()) return std::string();

    // 末尾が .json でなければ付与
    auto hasJsonExt = [](const std::string& s) {
        const char* ext = ".json";
        if (s.size() < 5) return false;
        return s.compare(s.size() - 5, 5, ext) == 0;
        };

    // すでにディレクトリ区切りが含まれているか
    auto hasDir = [](const std::string& s) {
        return s.find('/') != std::string::npos || s.find('\\') != std::string::npos;
        };

    std::string path = spec;
    if (!hasJsonExt(path)) {
        path += ".json";
    }
    if (!hasDir(path)) {
        path = "maps/" + path;
    }
    return path;
}

BattleStartCommand::BattleStartCommand(const nlohmann::json& evt)
    : mapSpec_(evt.value("map", std::string{}))
{
}

void BattleStartCommand::execute(ExecutionEngine& engine) {
    // 32x32 タイル前提（必要なら JSON に持たせる設計へ後で拡張）
    constexpr int TILE_W = 32;
    constexpr int TILE_H = 32;

    if (!engine.tileMap || !engine.battleManager) {
        std::cerr << "[BattleStartCommand] missing dependencies\n";
        return;
    }

    const std::string mapPath = normalizeMapPath(mapSpec_);
    if (mapPath.empty()) {
        std::cerr << "[BattleStartCommand] map path missing in script\n";
        return;
    }

    // ログは「受け取った生の指定」を表示（トラブルシュート用）
    std::cout << "[BattleManager] startBattle called with map: " << mapSpec_ << "\n";

    // tileset は固定配置を想定（必要なら JSON から読めるように拡張）
    const std::string tilesetBmp = "maps/tileset.bmp";

    if (!engine.tileMap->loadFromFile(mapPath, tilesetBmp, TILE_W, TILE_H)) {
        std::cerr << "[BattleStartCommand] map load failed (path used: " << mapPath << ")\n";
        return;
    }

    // マップのロードに成功したら内部状態を初期化
    engine.battleManager->startBattle(mapPath);
}