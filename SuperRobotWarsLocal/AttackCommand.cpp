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

    // 2) current cursor position
    int cx = cursor->getX();
    int cy = cursor->getY();

    // 3) compute reachable with four args
    auto reachable = computeReachable(map, cx, cy, cursor->getMoveRange());

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
        // perform attack
        bm->attack(cx, cy, targetX_, targetY_);
    }

    // 5) redraw & wait
    engine.redraw();
    engine.getInput()->waitKey();
}