// src/EnemyAttackCommand.cpp

#include "EnemyAttackCommand.h"
#include "ExecutionEngine.h"
#include "ReachableArea.h"
#include "ShowMessageCommand.h"
#include <SDL.h>
#include <iostream>

EnemyAttackCommand::EnemyAttackCommand(const nlohmann::json& evt)
    : unitId_(evt.value("unitId", 0))
    , attackRange_(evt.value("range", 1))
{
}

void EnemyAttackCommand::execute(ExecutionEngine& engine) {
    auto* bm = engine.getBattleManager();
    auto* map = engine.tileMap;
    auto* cursor = engine.getCursor();
    if (!bm || !map || !cursor) {
        std::cerr << "[EnemyAttackCommand] missing dependencies\n";
        return;
    }

    // 1) 自ユニット位置取得
    auto pos = bm->getUnitPosition(unitId_);

    // 2) 攻撃可能範囲を計算
    auto rangeTiles = computeReachable(map, pos[0], pos[1], attackRange_);

    // 3) 範囲内の味方ユニットをサーチ
    int targetId = -1;
    for (auto const& p : rangeTiles) {
        int id = bm->getUnitAt(p.first, p.second);
        if (id >= 0) {
            targetId = id;
            break;
        }
    }
    if (targetId < 0) {
        std::cerr << "[EnemyAttackCommand] no target in range\n";
        return;
    }

    // 4) ダメージ計算＆適用
    int dmg = bm->calculateDamage(unitId_, targetId);
    bm->applyDamage(targetId, dmg);

    // 5) ダメージ表示
    {
        nlohmann::json msgEvt;
        msgEvt["type"] = "showMessage";
        msgEvt["text"] = "敵" + std::to_string(unitId_) +
            "の攻撃！ ダメージ: " + std::to_string(dmg);
        ShowMessageCommand msgCmd(msgEvt);
        msgCmd.execute(engine);
    }
}