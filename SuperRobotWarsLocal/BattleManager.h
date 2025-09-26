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

    // ユニットレンダリング
    void renderUnits(SDLRenderer* renderer, int offsetX, int offsetY) const;

    // --- 以下を追加 ---
    // 指定座標にいるユニットIDを返す。いなければ -1 を返す
    int getUnitAt(int x, int y) const;

    // ダメージ計算（仮の stub 実装）
    int calculateDamage(int attackerId, int defenderId) const;

    // ダメージ適用（仮の stub 実装）
    void applyDamage(int unitId, int damage);

private:
    SDLRenderer* renderer_;
    struct Unit {
        int x, y;
        // 将来、HP などを追加
    };
    std::vector<Unit> units_;
};