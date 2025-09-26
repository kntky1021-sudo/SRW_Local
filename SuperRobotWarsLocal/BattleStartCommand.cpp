#include "BattleStartCommand.h"
#include "ExecutionEngine.h"
#include <iostream>

BattleStartCommand::BattleStartCommand(const nlohmann::json& evt)
    : mainUnitId_(evt.value("unitId", 0))
    , enemyIds_(evt.value("enemyIds", std::vector<int>{}))
    , mapPath_(evt.value("mapPath", ""))
{
    // フォールバックで "mapName" からも取れるように
    if (mapPath_.empty() && evt.contains("mapName")) {
        mapPath_ = evt["mapName"].get<std::string>();
    }

    // initPositions があれば読み込む
    if (evt.contains("initPositions") && evt["initPositions"].is_object()) {
        for (auto& it : evt["initPositions"].items()) {
            int id = std::stoi(it.key());
            auto arr = it.value();
            if (arr.is_array() && arr.size() == 2) {
                initPositions_[id] = {
                    arr[0].get<int>(),
                    arr[1].get<int>()
                };
            }
        }
    }
}

void BattleStartCommand::execute(ExecutionEngine& engine) {
    if (mapPath_.empty()) {
        std::cerr << "[BattleStartCommand] map path missing in script\n";
        return;
    }

    // マップ読み込み
    if (!engine.tileMap->load(mapPath_)) {
        std::cerr << "[TileMap] failed to load: " << mapPath_ << "\n";
        return;
    }

    // 初期位置のセット（initPositions_ の中身を BattleManager に反映する等）
    // …（既存実装）…

    engine.redraw();
    engine.waitKey();
}