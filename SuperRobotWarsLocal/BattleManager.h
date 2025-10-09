#pragma once

#include <vector>
#include <array>
#include <string>
#include "SDLRenderer.h"

// 前方宣言
class UIManager;

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

    // 指定座標にいるユニットIDを返す。いなければ -1 を返す
    int getUnitAt(int x, int y) const;

    // ダメージ計算（仮の stub 実装）
    int calculateDamage(int attackerId, int defenderId) const;

    // ダメージ適用（仮の stub 実装）
    void applyDamage(int unitId, int damage);

    // 攻撃実行メソッド
    // 攻撃者(attackerX, attackerY)から防御側(defenderX, defenderY)へ攻撃
    void attack(int attackerX, int attackerY, int defenderX, int defenderY);

    // ユニット追加
    int addUnit(int x, int y, bool isEnemy, const std::string& name = "Unit");

    // 行動済みフラグ管理
    void setUnitActed(int unitId, bool acted);
    bool hasUnitActed(int unitId) const;
    void resetAllActedFlags(bool enemyOnly = false);

    // ユニット情報取得
    bool isEnemyUnit(int unitId) const;
    std::string getUnitName(int unitId) const;
    int getUnitCount() const { return static_cast<int>(units_.size()); }

    // 敵味方の生存数取得
    int getAliveAllyCount() const;
    int getAliveEnemyCount() const;

    // 拡張: ユニット情報取得
    int getUnitHP(int unitId) const;
    int getUnitMaxHP(int unitId) const;

    // 経験値・レベル管理
    void addExperience(int unitId, int exp);
    bool checkLevelUp(int unitId);

private:
    SDLRenderer* renderer_;

    // 簡易ユニット構造体
    struct Unit {
        int x = 0;
        int y = 0;
        int hp = 100;
        bool isEnemy = false;     // 敵ユニットか
        bool hasActed = false;    // 行動済みか
        std::string name = "Unit"; // ユニット名
    };

    std::vector<Unit> units_;
};