#include "PlayerMoveCommand.h"
#include "ExecutionEngine.h"
#include "Cursor.h"
#include "BattleManager.h"
#include "TileMap.h"
#include <SDL.h>
#include <iostream>

PlayerMoveCommand::PlayerMoveCommand(const nlohmann::json& evt)
    : unitId_(evt.value("unitId", 0))
    , snapCursorToUnit_(evt.value("snapToUnit", true))
{
}

void PlayerMoveCommand::execute(ExecutionEngine& engine) {
    auto* cursor = engine.getCursor();
    auto* bm = engine.getBattleManager();
    auto* map = engine.tileMap;

    if (!cursor || !bm || !map) {
        std::cerr << "[PlayerMoveCommand] missing dependencies\n";
        return;
    }

    if (snapCursorToUnit_) {
        auto pos = bm->getUnitPosition(unitId_);
        cursor->setPosition(pos[0], pos[1]);
    }

    // 入力ループ：矢印で移動、Enter/ESC で決定
    bool decided = false;
    bool canceled = false;
    const int mapW = map->getMapWidth();
    const int mapH = map->getMapHeight();

    engine.redraw();
    SDL_Event e;
    while (!decided && !canceled) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                canceled = true;
            }
            else if (e.type == SDL_EVENT_KEY_DOWN) {
                auto sc = e.key.scancode;
                if (sc == SDL_SCANCODE_RETURN || sc == SDL_SCANCODE_KP_ENTER) {
                    decided = true;
                }
                else if (sc == SDL_SCANCODE_ESCAPE) {
                    canceled = true;
                }
                else if (sc == SDL_SCANCODE_UP) {
                    cursor->move(0, -1, mapW, mapH);
                }
                else if (sc == SDL_SCANCODE_DOWN) {
                    cursor->move(0, +1, mapW, mapH);
                }
                else if (sc == SDL_SCANCODE_LEFT) {
                    cursor->move(-1, 0, mapW, mapH);
                }
                else if (sc == SDL_SCANCODE_RIGHT) {
                    cursor->move(+1, 0, mapW, mapH);
                }
                engine.redraw();
            }
        }
        SDL_Delay(5);
    }

    if (canceled) {
        std::cout << "[PlayerMoveCommand] canceled by user\n";
        return;
    }

    // 決定位置へユニット移動
    const int cx = cursor->getX();
    const int cy = cursor->getY();
    bm->moveUnit(unitId_, cx, cy);
    engine.redraw();
}