#include "ExecutionEngine.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>

#include "ShowMessageCommand.h"
#include "BattleStartCommand.h"
#include "MoveCommand.h"
#include "PlayerMoveCommand.h"

ExecutionEngine::ExecutionEngine(UIManager* uiMgr,
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
    , camera_(tileMap_->getMapWidth()* tileMap_->getTileWidth(),
        tileMap_->getMapHeight()* tileMap_->getTileHeight(),
        windowW,
        windowH)
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
}


void ExecutionEngine::redraw() {
    // SDLRenderer 経由で SDL_Renderer* を取得
    auto* rdr = ui->getRenderer();
    auto* sdlR = rdr->getSDLRenderer();

    // 1) ゲーム背景用のクリア色をセット（たとえば黒）
    SDL_SetRenderDrawColor(sdlR,
        0,   // R
        0,   // G
        0,   // B
        SDL_ALPHA_OPAQUE);

    // 2) クリア
    rdr->clear();

    // 3) カメラ更新（カーソルを中央に追従）
    camera_.update(cursor->getX() * tileMap->getTileWidth(),
        cursor->getY() * tileMap->getTileHeight());
    const int ox = camera_.getOffsetX();
    const int oy = camera_.getOffsetY();

    // 4) マップ・ユニット・カーソルを描画
    if (tileMap) {
        tileMap->render(ox, oy);
    }
    if (battleManager) {
        battleManager->renderUnits(rdr, ox, oy);
    }
    if (cursor) {
        cursor->render(ox, oy);
    }

    // 5) 最後に画面更新
    rdr->present();
}
