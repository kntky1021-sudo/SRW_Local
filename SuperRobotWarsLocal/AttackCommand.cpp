// src/AttackCommand.cpp

#include "AttackCommand.h"
#include "ExecutionEngine.h"
#include "ReachableArea.h"
#include "ShowMessageCommand.h"
#include <SDL.h>
#include <nlohmann/json.hpp>
#include <iostream>

AttackCommand::AttackCommand(const nlohmann::json& evt)
    : unitId_(evt.value("unitId", 0))
    , attackRange_(evt.value("range", 1))
{
}

void AttackCommand::execute(ExecutionEngine& engine) {
    auto* bm = engine.getBattleManager();
    auto* map = engine.tileMap;
    auto* cursor = engine.getCursor();
    if (!bm || !map || !cursor) {
        std::cerr << "[AttackCommand] dependencies missing\n";
        return;
    }

    // 1) ユニット位置取得＆カーソルをスナップ
    auto pos = bm->getUnitPosition(unitId_);
    cursor->setPosition(pos[0], pos[1]);

    // 2) 攻撃可能範囲を計算＆ハイライト
    auto rangeTiles = computeReachable(map, pos[0], pos[1], attackRange_);
    engine.setHighlightTiles(rangeTiles);
    engine.redraw();

    // 3) 範囲内のみカーソル移動 → ENTER で決定
    bool decided = false;
    while (!decided) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_KEY_DOWN) {
                int dx = 0, dy = 0;
                switch (e.key.scancode) {
                case SDL_SCANCODE_UP:    dy = -1; break;
                case SDL_SCANCODE_DOWN:  dy = +1; break;
                case SDL_SCANCODE_LEFT:  dx = -1; break;
                case SDL_SCANCODE_RIGHT: dx = +1; break;
                case SDL_SCANCODE_RETURN:
                    decided = true;
                    continue;
                default:
                    break;
                }
                if (!decided && (dx || dy)) {
                    int nx = cursor->getX() + dx;
                    int ny = cursor->getY() + dy;
                    for (auto const& p : rangeTiles) {
                        if (p.first == nx && p.second == ny) {
                            cursor->move(dx, dy,
                                map->getMapWidth(),
                                map->getMapHeight());
                            break;
                        }
                    }
                }
                engine.redraw();
            }
        }
        SDL_Delay(5);
    }

    // 4) 対象ユニット取得
    int tx = cursor->getX();
    int ty = cursor->getY();
    int targetId = bm->getUnitAt(tx, ty);
    if (targetId < 0) {
        std::cerr << "[AttackCommand] no unit at " << tx << "," << ty << "\n";
        engine.setHighlightTiles({});
        engine.redraw();
        return;
    }

    // 5) ダメージ計算＆適用
    int dmg = bm->calculateDamage(unitId_, targetId);
    bm->applyDamage(targetId, dmg);

    // 6) ダメージ表示（ShowMessageCommand を使って確実に画面に出す）
    {
        nlohmann::json msgEv;
        msgEv["type"] = "showMessage";
        msgEv["text"] = "ダメージ: " + std::to_string(dmg);
        ShowMessageCommand msgCmd(msgEv);
        msgCmd.execute(engine);
    }

    // 7) ハイライト解除＆再描画
    engine.setHighlightTiles({});
    engine.redraw();
}