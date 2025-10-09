#pragma once

#include <string>
#include "SDLRenderer.h"
#include "BattleCalculator.h"
#include <SDL3_ttf/SDL_ttf.h>

class Unit;
class UIManager;
class TextureManager;  // ← 追加

/// 戦闘UI管理クラス
/// 戦闘プレビュー、結果表示、戦闘デモ画面を担当
class BattleUIManager {
public:
    BattleUIManager(SDLRenderer* renderer, UIManager* uiManager);
    ~BattleUIManager();

    /// テクスチャマネージャーをセット
    void setTextureManager(TextureManager* texMgr) { texMgr_ = texMgr; }

    /// 戦闘プレビュー画面を表示
    void showBattlePreview(
        const Unit* attacker,
        const Unit* defender,
        int hitRate,
        int critRate);

    /// ダメージ表示
    void showDamage(int damage, bool isCritical, bool isMiss);

    /// 戦闘結果画面を表示
    void showBattleResult(
        const BattleResult& result,
        const std::string& attackerName,
        const std::string& defenderName);

    /// 戦闘デモ画面の初期化
    void initBattleDemo(const Unit* attacker, const Unit* defender);

    /// 戦闘デモ画面を描画
    void renderBattleDemo();

    /// 攻撃アニメーション再生
    void playAttackAnimation(bool isAttacker);

    /// ダメージエフェクト再生
    void playDamageEffect(int x, int y, int damage);

private:
    SDLRenderer* renderer_;
    UIManager* uiManager_;
    TTF_Font* font_;
    TextureManager* texMgr_;  // ← 追加

    // 戦闘デモ用の情報
    const Unit* demoAttacker_;
    const Unit* demoDefender_;

    // アニメーション用の状態
    int animationFrame_;
    bool isAnimating_;

    // UI描画ヘルパー
    void drawBox(int x, int y, int w, int h, bool filled = false);
    void drawTextCentered(const std::string& text, int x, int y, int w);
    void drawBar(int x, int y, int w, int h, float ratio,
        Uint8 r, Uint8 g, Uint8 b);
};