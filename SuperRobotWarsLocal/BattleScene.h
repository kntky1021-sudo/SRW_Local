#pragma once

#include "BattleCalculator.h"
#include <memory>

class Unit;
class Weapon;
class UIManager;
class SDLRenderer;

/// 戦闘シーン管理
/// 戦闘アニメーション・結果表示を担当
class BattleScene {
public:
    explicit BattleScene(UIManager* ui, SDLRenderer* renderer);

    /// @brief 戦闘を実行して結果を表示
    /// @param attacker 攻撃側
    /// @param defender 防御側
    /// @param weapon 使用武器
    /// @param terrain 地形
    /// @return 戦闘結果
    BattleResult performBattle(
        Unit* attacker,
        Unit* defender,
        Weapon* weapon,
        char terrain = 'L'
    );

private:
    UIManager* ui_;
    SDLRenderer* renderer_;

    /// 戦闘前の表示（ユニット情報、命中率など）
    void showBattlePreview(
        const Unit* attacker,
        const Unit* defender,
        const Weapon* weapon,
        int hitRate
    );

    /// 攻撃アニメーション
    void playAttackAnimation(
        const Unit* attacker,
        const Weapon* weapon
    );

    /// ダメージ表示
    void showDamage(
        const Unit* target,
        int damage,
        bool isCritical
    );

    /// 戦闘結果表示
    void showBattleResult(
        const BattleResult& result,
        const Unit* attacker,
        const Unit* defender
    );
};