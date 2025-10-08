#include "ExecutionEngine.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
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
    int            windowW,
    int            windowH)
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
        std::cerr << "[ExecutionEngine] Cannot open script: "
            << scriptPath << "\n";
        return;
    }

    nlohmann::json j;
    try {
        ifs >> j;
    }
    catch (const nlohmann::json::exception& e) {
        std::cerr << "[ExecutionEngine] JSON parse error: "
            << e.what() << "\n";
        return;
    }
    if (!j.is_array()) {
        std::cerr << "[ExecutionEngine] Script is not a JSON array\n";
        return;
    }

    // コマンドリストをクリアして新規構築
    commands_.clear();
    commandPhases_.clear();
    for (auto& evt : j) {
        auto type = evt.value("type", "");
        Phase ph = Phase::Script;

        if (type == "showMessage") {
            commands_.push_back(
                std::make_unique<ShowMessageCommand>(evt));
        }
        else if (type == "battleStart") {
            commands_.push_back(
                std::make_unique<BattleStartCommand>(evt));
        }
        else if (type == "move") {
            commands_.push_back(
                std::make_unique<MoveCommand>(evt));
        }
        else if (type == "playerMove") {
            ph = Phase::PlayerMove;
            commands_.push_back(
                std::make_unique<PlayerMoveCommand>(evt));
        }
        else if (type == "attack") {
            ph = Phase::PlayerAttack;
            commands_.push_back(
                std::make_unique<AttackCommand>(evt));
        }
        else if (type == "enemyMove") {
            ph = Phase::EnemyMove;
            commands_.push_back(
                std::make_unique<EnemyMoveCommand>(evt));
        }
        else if (type == "enemyAttack") {
            ph = Phase::EnemyAttack;
            commands_.push_back(
                std::make_unique<EnemyAttackCommand>(evt));
        }
        else {
            std::cerr << "[ExecutionEngine] Unknown command: "
                << type << "\n";
            continue;
        }
        commandPhases_.push_back(ph);
    }

    // 読み込んだコマンド数を出力
    std::cout << "[ExecutionEngine] commands loaded: "
        << commands_.size() << "\n";

    // 実行ループ
    currentIndex_ = 0;
    while (currentIndex_ < commands_.size()) {
        currentPhase_ = commandPhases_[currentIndex_];
        std::cout << "[ExecutionEngine] Phase="
            << static_cast<int>(currentPhase_)
            << " Executing command#"
            << currentIndex_ << "\n";

        commands_[currentIndex_]->execute(*this);
        ++currentIndex_;
    }

    // 最後に描画＆キー待ち
    redraw();
    if (input) {
        input->waitKey();
    }
}

void ExecutionEngine::setHighlightTiles(
    const std::vector<std::pair<int, int>>& tiles)
{
    highlightTiles_ = tiles;
}

void ExecutionEngine::redraw() {
    // クリア
    auto* sdlRen = ui->getRenderer()->getSDLRenderer();
    SDL_SetRenderDrawColor(sdlRen, 0, 0, 0, SDL_ALPHA_OPAQUE);
    ui->getRenderer()->clear();

    // カメラ更新
    camera_.update(
        cursor->getX() * tileMap->getTileWidth(),
        cursor->getY() * tileMap->getTileHeight());

    int ox = camera_.getOffsetX();
    int oy = camera_.getOffsetY();

    // マップ描画
    tileMap->render(ox, oy);

    // ハイライト矩形描画
    if (!highlightTiles_.empty()) {
        SDL_SetRenderDrawBlendMode(sdlRen, SDL_BLENDMODE_BLEND);
        if (currentPhase_ == Phase::EnemyMove ||
            currentPhase_ == Phase::EnemyAttack)
        {
            SDL_SetRenderDrawColor(sdlRen, 255, 0, 0, 128);
        }
        else {
            SDL_SetRenderDrawColor(sdlRen, 0, 0, 255, 128);
        }

        int tw = tileMap->getTileWidth();
        int th = tileMap->getTileHeight();
        for (auto& p : highlightTiles_) {
            SDL_FRect frect{
                static_cast<float>(p.first * tw - ox),
                static_cast<float>(p.second * th - oy),
                static_cast<float>(tw),
                static_cast<float>(th)
            };
            SDL_RenderFillRect(sdlRen, &frect);
        }
        SDL_SetRenderDrawBlendMode(sdlRen, SDL_BLENDMODE_NONE);
    }

    // ユニット描画
    if (battleManager) {
        battleManager->renderUnits(ui->getRenderer(), ox, oy);
    }

    // カーソル描画
    if (cursor) {
        cursor->render(ox, oy);
    }

    // 最後に画面反映
    ui->getRenderer()->present();
}