#include "ExecutionEngine.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <SDL.h>

#include "ShowMessageCommand.h"
#include "BattleStartCommand.h"
#include "MoveCommand.h"
#include "PlayerMoveCommand.h"

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
{
}

void ExecutionEngine::run(const std::string& scriptPath) {
    std::ifstream ifs(scriptPath);
    if (!ifs) {
        std::cerr << "[ExecutionEngine] Failed to open: "
            << scriptPath << "\n";
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

    commands_.clear();
    for (auto& evt : j) {
        auto type = evt.value("type", "");
        if (type == "showMessage") {
            commands_.push_back(std::make_unique<ShowMessageCommand>(evt));
        }
        else if (type == "battleStart") {
            commands_.push_back(std::make_unique<BattleStartCommand>(evt));
        }
        else if (type == "move") {
            commands_.push_back(std::make_unique<MoveCommand>(evt));
        }
        else if (type == "playerMove") {
            commands_.push_back(std::make_unique<PlayerMoveCommand>(evt));
        }
    }

    currentIndex_ = 0;
    while (currentIndex_ < commands_.size()) {
        commands_[currentIndex_]->execute(*this);
        ++currentIndex_;
    }

    // 全コマンド実行後に最後の画面を描画し、任意キー押下まで待機
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
    auto* rdr = ui->getRenderer();
    auto* sdlR = rdr->getSDLRenderer();

    // 1) ゲーム背景用クリア色を黒にセット
    SDL_SetRenderDrawColor(sdlR, 0, 0, 0, SDL_ALPHA_OPAQUE);
    rdr->clear();

    // 2) カメラ更新
    camera_.update(
        cursor->getX() * tileMap->getTileWidth(),
        cursor->getY() * tileMap->getTileHeight());
    int ox = camera_.getOffsetX();
    int oy = camera_.getOffsetY();

    // 3) マップ描画
    if (tileMap) {
        tileMap->render(ox, oy);
    }

    // 4) ハイライトタイル描画 (半透明青)
    if (!highlightTiles_.empty()) {
        SDL_SetRenderDrawBlendMode(sdlR, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(sdlR, 0, 0, 255, 128);

        int tw = tileMap->getTileWidth();
        int th = tileMap->getTileHeight();
        for (auto& p : highlightTiles_) {
            // 整数計算後に SDL_FRect へキャスト
            SDL_FRect frect{
                static_cast<float>(p.first * tw - ox),
                static_cast<float>(p.second * th - oy),
                static_cast<float>(tw),
                static_cast<float>(th)
            };
            SDL_RenderFillRect(sdlR, &frect);
        }

        // ブレンドモードを戻す
        SDL_SetRenderDrawBlendMode(sdlR, SDL_BLENDMODE_NONE);
    }

    // 5) ユニット描画
    if (battleManager) {
        battleManager->renderUnits(rdr, ox, oy);
    }

    // 6) カーソル描画
    if (cursor) {
        cursor->render(ox, oy);
    }

    // 7) プレゼンテーション
    rdr->present();
}