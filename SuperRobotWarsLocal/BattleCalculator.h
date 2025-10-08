#pragma once

#include <memory>

class Unit;
class Weapon;

/// 戦闘計算結果
struct BattleResult {
    int damage;              // 与ダメージ
    bool isCritical;         // クリティカルヒット
    bool isCountered;        // 反撃されたか
    int counterDamage;       // 反撃ダメージ
    bool attackerDestroyed;  // 攻撃側撃破
    bool defenderDestroyed;  // 防御側撃破
    int expGained;           // 獲得経験値
};

/// 戦闘計算クラス
/// SFC版スパロボ風のダメージ計算を実装
class BattleCalculator {
public:
    /// @brief 戦闘を実行し結果を返す
    /// @param attacker 攻撃側ユニット
    /// @param defender 防御側ユニット
    /// @param weapon 使用武器
    /// @param terrain 地形タイプ（'L':陸、'S':海、'A':空、'W':宇宙）
    /// @return 戦闘結果
    static BattleResult execute(
        Unit* attacker,
        Unit* defender,
        Weapon* weapon,
        char terrain = 'L'
    );

    /// @brief 命中率を計算
    /// @return 命中率（0-100%）
    static int calculateHitRate(
        const Unit* attacker,
        const Unit* defender,
        const Weapon* weapon,
        char terrain
    );

    /// @brief 回避率を計算
    /// @return 回避率（0-100%）
    static int calculateEvadeRate(
        const Unit* defender,
        char terrain
    );

    /// @brief クリティカル率を計算
    /// @return クリティカル率（0-100%）
    static int calculateCriticalRate(
        const Unit* attacker,
        const Weapon* weapon
    );

    /// @brief ダメージを計算（命中時）
    /// @param isCritical クリティカルヒットか
    /// @return 最終ダメージ
    static int calculateDamage(
        const Unit* attacker,
        const Unit* defender,
        const Weapon* weapon,
        char terrain,
        bool isCritical
    );

    /// @brief 地形適応による補正値を取得
    /// @param adaptation 地形適応（'S', 'A', 'B', 'C', 'D'）
    /// @return 補正率（0.5～1.2）
    static float getTerrainModifier(char adaptation);

    /// @brief 獲得経験値を計算
    /// @param attacker 攻撃側ユニット
    /// @param defender 防御側ユニット
    /// @param damaged ダメージを与えたか
    /// @param destroyed 撃破したか
    /// @return 経験値
    static int calculateExperience(
        const Unit* attacker,
        const Unit* defender,
        bool damaged,
        bool destroyed
    );

private:
    // 乱数生成（0-99の整数）
    static int random100();
};