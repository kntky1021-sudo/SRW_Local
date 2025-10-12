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

    // ユニットID生成（簡易版：unitIdから文字列生成）
    std::string unitIdStr = "unit_" + std::to_string(unitId_);

    // ユニット取得
    Unit* unit = bm->getUnitById(unitIdStr);
    if (!unit) {
        std::cerr << "[MoveCommand] Unit not found: " << unitIdStr << "\n";
        return;
    }

    // 開始位置にカーソルを置き、表示・キー待ち
    int startX = unit->getX();
    int startY = unit->getY();
    cursor->setPosition(startX, startY);
    engine.redraw();
    engine.waitKey();

    // 移動→再描画→キー待ち
    bm->moveUnit(unitIdStr, toPos_[0], toPos_[1]);
    cursor->setPosition(toPos_[0], toPos_[1]);
    engine.redraw();
    engine.waitKey();
}