#include "ExecutionEngine.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <SDL.h>

#include "ShowMessageCommand.h"
#include "BattleStartCommand.h"
#include "MoveCommand.h"
#include "PlayerMoveCommand.h"
#include "AttackCommand.h"
#include "EnemyMoveCommand.h"
#include "EnemyAttackCommand.h"

ExecutionEngine::ExecutionEngine(
    UIManager* uiMgr,
    InputManager* inputMgr,
    TileMap* tileMap_,
    Cursor* cursor_,
    BattleManager* battleMgr,
    int windowW,
    int windowH)
    : ui(uiMgr)
    , input(inputMgr)
    , tileMap(tileMap_)
    , cursor(cursor_)
    , battleManager(battleMgr)
    , currentIndex_(0)
    , camera_(
        tileMap_->getMapWidth()* tileMap_->getTileWidth(),
        tileMap_->getMapHeight()* tileMap_->getTileHeight(),
        windowW,
        windowH)
    , currentPhase_(Phase::Script)
{
}

void ExecutionEngine::run(const std::string& scriptPath) {
    std::ifstream ifs(scriptPath);
    if (!ifs) {
        std::cerr << "[ExecutionEngine] Failed to open: " << scriptPath << "\n";
        return;
    }

    nlohmann::json j;
    try {
        ifs >> j;
    }
    catch (const nlohmann::json::exception& e) {
        std::cerr << "[ExecutionEngine] JSON parse error in "
            << scriptPath << ": " << e.what() << "\n";
        return;
    }
    if (!j.is_array()) {
        std::cerr << "[ExecutionEngine] Script is not JSON array\n";
        return;
    }

    // キュー構築
    commands_.clear();
    commandPhases_.clear();

    for (auto& evt : j) {
        auto type = evt.value("type", "");
        Phase ph = Phase::Script;

        if (type == "showMessage") {
            ph = Phase::Script;
            commands_.push_back(std::make_unique<ShowMessageCommand>(evt));
        }
        else if (type == "battleStart") {
            ph = Phase::Script;
            commands_.push_back(std::make_unique<BattleStartCommand>(evt));
        }
        else if (type == "move") {
            ph = Phase::Script;
            commands_.push_back(std::make_unique<MoveCommand>(evt));
        }
        else if (type == "playerMove") {
            ph = Phase::PlayerMove;
            commands_.push_back(std::make_unique<PlayerMoveCommand>(evt));
        }
        else if (type == "attack") {
            ph = Phase::PlayerAttack;
            commands_.push_back(std::make_unique<AttackCommand>(evt));
        }
        else if (type == "enemyMove") {
            ph = Phase::EnemyMove;
            commands_.push_back(std::make_unique<EnemyMoveCommand>(evt));
        }
        else if (type == "enemyAttack") {
            ph = Phase::EnemyAttack;
            commands_.push_back(std::make_unique<EnemyAttackCommand>(evt));
        }
        else {
            std::cerr << "[ExecutionEngine] Unknown command type: " << type << "\n";
            continue;
        }

        commandPhases_.push_back(ph);
    }

    // 実行ループ
    currentIndex_ = 0;
    while (currentIndex_ < commands_.size()) {
        // 実行前に currentPhase_ を更新
        currentPhase_ = commandPhases_[currentIndex_];

        // フェイズ＆インデックスをログ出力
        std::cout
            << "[ExecutionEngine] Phase=" << static_cast<int>(currentPhase_)
            << " Executing command#" << currentIndex_ << "\n";

        commands_[currentIndex_]->execute(*this);
        ++currentIndex_;
    }

    // 最終描画＆キー待ち
    redraw();
    if (input) {
        input->waitKey();
    }
}