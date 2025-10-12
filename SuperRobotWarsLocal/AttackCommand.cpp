#include "AttackCommand.h"
#include "ExecutionEngine.h"
#include "ReachableArea.h"
#include <iostream>

AttackCommand::AttackCommand(const nlohmann::json& evt)
    : targetX_(evt.value("targetX", -1))
    , targetY_(evt.value("targetY", -1))
{
}

void AttackCommand::execute(ExecutionEngine& engine) {
    // 1) grab engine internals via getters
    Cursor* cursor = engine.getCursor();
    BattleManager* bm = engine.getBattleManager();
    TileMap* map = engine.getTileMap();

    if (!cursor || !bm || !map) {
        std::cerr << "[AttackCommand] missing dependencies\n";
        return;
    }

    // 2) current cursor position
    int cx = cursor->getX();
    int cy = cursor->getY();

    // カーソル位置のユニットを取得
    Unit* attacker = bm->getUnitAt(cx, cy);
    if (!attacker) {
        std::cerr << "[AttackCommand] No attacker at cursor position\n";
        return;
    }

    // 3) compute reachable with four args
    const int attackRange = attacker->getAttackRange();
    auto reachable = computeReachable(map, cx, cy, attackRange);

    // 4) check target is in reachable
    bool canAttack = false;
    for (auto& p : reachable) {
        if (p.first == targetX_ && p.second == targetY_) {
            canAttack = true;
            break;
        }
    }

    if (!canAttack) {
        std::cerr << "[AttackCommand] target out of range\n";
    }
    else {
        // ターゲット取得
        Unit* defender = bm->getUnitAt(targetX_, targetY_);
        if (!defender) {
            std::cerr << "[AttackCommand] No defender at target position\n";
            return;
        }

        // perform attack
        bm->attack(attacker->getId(), defender->getId());
    }

    // 5) redraw & wait
    engine.redraw();
    engine.waitKey();
}