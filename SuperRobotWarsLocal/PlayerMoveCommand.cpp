#include "PlayerMoveCommand.h"
#include "ExecutionEngine.h"
#include "Cursor.h"
#include "BattleManager.h"
#include "TileMap.h"
#include "ReachableArea.h"
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

    // ユニット初期位置にスナップ
    auto pos = bm->getUnitPosition(unitId_);
    if (snapCursorToUnit_) {
        cursor->setPosition(pos[0], pos[1]);
    }

    // 移動可能範囲を計算（仮の移動力 5 で計算）
    const int maxMove = 5;
    auto reachable = computeReachable(map, pos[0], pos[1], maxMove);
    engine.setHighlightTiles(reachable);

    // 最初の描画（範囲＋ユニット＋カーソル）
    engine.redraw();

    // 範囲内判定ヘルパー
    auto isInRange = [&](int x, int y) {
        for (auto& p : reachable) {
            if (p.first == x && p.second == y) return true;
        }
        return false;
        };

    bool decided = false;
    bool canceled = false;
    const int mapW = map->getMapWidth();
    const int mapH = map->getMapHeight();

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
                else {
                    int dx = 0, dy = 0;
                    if (sc == SDL_SCANCODE_UP)    dy = -1;
                    else if (sc == SDL_SCANCODE_DOWN)  dy = +1;
                    else if (sc == SDL_SCANCODE_LEFT)  dx = -1;
                    else if (sc == SDL_SCANCODE_RIGHT) dx = +1;

                    // 範囲内ならカーソル移動
                    if ((dx != 0 || dy != 0)) {
                        int nx = cursor->getX() + dx;
                        int ny = cursor->getY() + dy;
                        if (nx >= 0 && ny >= 0 && nx < mapW && ny < mapH
                            && isInRange(nx, ny))
                        {
                            cursor->move(dx, dy, mapW, mapH);
                        }
                    }
                }
                engine.redraw();
            }
        }
        SDL_Delay(5);
    }

    // ハイライトクリア
    engine.setHighlightTiles({});

    if (canceled) {
        std::cout << "[PlayerMoveCommand] canceled by user\n";
        return;
    }

    // 決定マスへユニット移動
    const int cx = cursor->getX();
    const int cy = cursor->getY();
    bm->moveUnit(unitId_, cx, cy);
    engine.redraw();
}