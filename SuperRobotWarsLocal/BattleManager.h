#pragma once
#include <vector>
#include <array>
#include <string>
#include "SDLRenderer.h"

class BattleManager {
public:
    explicit BattleManager(SDLRenderer* renderer);
    ~BattleManager();

    // マップ開始用（コマンドから呼ばれる）
    void startBattle(const std::string& mapName);

    // ユニット移動
    void moveUnit(int unitId, int x, int y);

    // 現在位置取得
    std::array<int, 2> getUnitPosition(int unitId) const;

    // カメラオフセット指定描画
    void renderUnits(SDLRenderer* renderer,
        int offsetX,
        int offsetY) const;

private:
    SDLRenderer* renderer_;
    struct Unit { int x, y; };
    std::vector<Unit> units_;
};