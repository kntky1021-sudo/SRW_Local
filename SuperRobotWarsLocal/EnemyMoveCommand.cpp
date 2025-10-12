#include "EnemyMoveCommand.h"
#include "ExecutionEngine.h"
#include "ReachableArea.h"
#include <SDL.h>
#include <iostream>

EnemyMoveCommand::EnemyMoveCommand(const nlohmann::json& evt)
    : unitId_(evt.value("unitId", 0))
    , moveRange_(evt.value("range", 5))
{
}

void EnemyMoveCommand::execute(ExecutionEngine& engine) {
    auto* bm = engine.getBattleManager();
    auto* map = engine.getTileMap();

    if (!bm || !map) {
        std::cerr << "[EnemyMoveCommand] missing dependencies\n";
        return;
    }

    // ユニットID生成
    std::string unitIdStr = "unit_" + std::to_string(unitId_);

    // ユニット取得
    Unit* unit = bm->getUnitById(unitIdStr);
    if (!unit) {
        std::cerr << "[EnemyMoveCommand] Unit not found: " << unitIdStr << "\n";
        return;
    }

    // 現在位置
    int posX = unit->getX();
    int posY = unit->getY();

    // 移動可能範囲を計算
    auto reachable = computeReachable(map, posX, posY, moveRange_);

    // 例: 現在の位置以外の最初のタイルへ移動
    if (reachable.size() > 1) {
        auto target = reachable[1];
        bm->moveUnit(unitIdStr, target.first, target.second);
        engine.redraw();
        SDL_Delay(400);
    }
    else {
        std::cerr << "[EnemyMoveCommand] no reachable tile\n";
    }
}