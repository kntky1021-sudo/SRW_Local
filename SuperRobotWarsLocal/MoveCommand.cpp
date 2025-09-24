#include "MoveCommand.h"
#include "ExecutionEngine.h"
#include "Cursor.h"
#include "BattleManager.h"
#include <iostream>

MoveCommand::MoveCommand(const nlohmann::json& evt)
    : unitId_(-1)
    , toPos_{ 0, 0 }
{
    if (evt.contains("unitId")) {
        unitId_ = evt["unitId"].get<int>();
    }
    if (evt.contains("to") && evt["to"].is_array() && evt["to"].size() == 2) {
        toPos_[0] = evt["to"][0].get<int>();
        toPos_[1] = evt["to"][1].get<int>();
    }
}

void MoveCommand::execute(ExecutionEngine& engine) {
    auto* cursor = engine.getCursor();
    auto* bm = engine.getBattleManager();
    if (!cursor || !bm) {
        std::cerr << "[MoveCommand] dependencies not set\n";
        return;
    }

    // 開始位置にカーソルを置き、表示・キー待ち
    auto start = bm->getUnitPosition(unitId_);
    cursor->setPosition(start[0], start[1]);
    engine.redraw();
    engine.waitKey();

    // 移動→再描画→キー待ち
    bm->moveUnit(unitId_, toPos_[0], toPos_[1]);
    cursor->setPosition(toPos_[0], toPos_[1]);
    engine.redraw();
    engine.waitKey();
}