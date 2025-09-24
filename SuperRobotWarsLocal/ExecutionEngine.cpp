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
    auto* rdr = ui->getRenderer();
    rdr->clear();

    // カーソル位置(px)をターゲットにしてカメラ更新
    camera_.update(cursor->getX() * tileMap->getTileWidth(),
        cursor->getY() * tileMap->getTileHeight());

    const int ox = camera_.getOffsetX();
    const int oy = camera_.getOffsetY();

    if (tileMap)       tileMap->render(ox, oy);
    if (battleManager) battleManager->renderUnits(rdr, ox, oy);
    if (cursor)        cursor->render(ox, oy);

    rdr->present();
}