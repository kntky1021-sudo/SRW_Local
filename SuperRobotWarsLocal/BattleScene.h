#pragma once

#include "BattleCalculator.h"
#include <memory>

class Unit;
struct WeaponData;
class UIManager;
class SDLRenderer;
class AudioManager;  // ← 追加

class BattleScene {
public:
    explicit BattleScene(UIManager* ui, SDLRenderer* renderer);

    // AudioManager設定（追加）
    void setAudioManager(AudioManager* audioMgr) { audioManager_ = audioMgr; }

    BattleResult performBattle(
        Unit* attacker,
        Unit* defender,
        const WeaponData* weapon,
        char terrain = 'L'
    );

private:
    UIManager* ui_;
    SDLRenderer* renderer_;
    AudioManager* audioManager_;  // ← 追加

    /// 戦闘前の表示（ユニット情報、命中率など）
    void showBattlePreview(
        const Unit* attacker,
        const Unit* defender,
        const WeaponData* weapon,
        int hitRate
    );

    /// 攻撃アニメーション
    void playAttackAnimation(
        const Unit* attacker,
        const WeaponData* weapon
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