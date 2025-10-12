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
    auto* map = engine.getTileMap();
    auto* cursor = engine.getCursor();

    if (!bm || !map || !cursor) {
        std::cerr << "[EnemyAttackCommand] missing dependencies\n";
        return;
    }

    // ユニットID生成
    std::string unitIdStr = "unit_" + std::to_string(unitId_);

    // 自ユニット取得
    Unit* attacker = bm->getUnitById(unitIdStr);
    if (!attacker) {
        std::cerr << "[EnemyAttackCommand] Attacker not found: " << unitIdStr << "\n";
        return;
    }

    // 自ユニット位置取得
    int posX = attacker->getX();
    int posY = attacker->getY();

    // 攻撃可能範囲を計算
    auto rangeTiles = computeReachable(map, posX, posY, attackRange_);

    // 範囲内の味方ユニットをサーチ
    std::string targetId;
    for (auto const& p : rangeTiles) {
        Unit* target = bm->getUnitAt(p.first, p.second);
        if (target && target->getId() != unitIdStr && target->getTeam() == Team::Ally) {
            targetId = target->getId();
            break;
        }
    }

    if (targetId.empty()) {
        std::cerr << "[EnemyAttackCommand] no target in range\n";
        return;
    }

    // 攻撃実行
    bm->attack(unitIdStr, targetId);

    // ダメージ表示
    {
        nlohmann::json msgEvt;
        msgEvt["type"] = "showMessage";
        msgEvt["text"] = "Enemy " + unitIdStr + " attacked!";
        ShowMessageCommand msgCmd(msgEvt);
        msgCmd.execute(engine);
    }
}