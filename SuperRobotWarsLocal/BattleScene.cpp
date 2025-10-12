#include "BattleScene.h"
#include "UIManager.h"
#include "SDLRenderer.h"
#include "Unit.h"
#include "UnitData.h"
#include "BattleCalculator.h"
#include "BattleUIManager.h"
#include <SDL.h>
#include <iostream>
#include <sstream>
#include "TextureManager.h"
#include "AudioManager.h" 

BattleScene::BattleScene(UIManager* ui, SDLRenderer* renderer)
    : ui_(ui)
    , renderer_(renderer)
    , audioManager_(nullptr)  // ← 追加
{
}

BattleResult BattleScene::performBattle(
    Unit* attacker,
    Unit* defender,
    const WeaponData* weapon,
    char terrain)
{
    if (!attacker || !defender) {
        std::cerr << "[BattleScene] Invalid parameters\n";
        return BattleResult{};
    }

    std::cout << "[BattleScene] Starting battle...\n";

    // TextureManager作成と画像読み込み
    TextureManager texMgr(renderer_);

    // テスト用画像を読み込み
    std::string attackerTexId = "unit_" + attacker->getId();
    std::string defenderTexId = "unit_" + defender->getId();

    // 味方・敵それぞれのテスト画像を読み込み
    texMgr.loadTexture(attackerTexId, "assets/units/test_ally.bmp");
    texMgr.loadTexture(defenderTexId, "assets/units/test_enemy.bmp");

    // BattleUIManager作成
    BattleUIManager battleUI(renderer_, ui_);
    battleUI.setTextureManager(&texMgr);

    // 1) 戦闘前プレビュー表示
    int hitRate = BattleCalculator::calculateHitRate(attacker, defender, weapon, terrain);
    int critRate = BattleCalculator::calculateCriticalRate(attacker, weapon);

    battleUI.showBattlePreview(attacker, defender, hitRate, critRate);

    // キー入力待ち
    SDL_Event e;
    bool waiting = true;
    while (waiting) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_KEY_DOWN || e.type == SDL_EVENT_QUIT) {
                waiting = false;
                break;
            }
        }
        SDL_Delay(16);
    }

    // 2) 戦闘デモ初期化と表示
    battleUI.initBattleDemo(attacker, defender);
    battleUI.renderBattleDemo();
    SDL_Delay(1500);

    // 3) 戦闘計算実行
    BattleResult result = BattleCalculator::execute(attacker, defender, weapon, terrain);

    // 4) 攻撃アニメーション
    battleUI.playAttackAnimation(true);
    battleUI.renderBattleDemo();
    SDL_Delay(800);

    // 5) ダメージ表示
    battleUI.showDamage(result.damage, result.isCritical, result.damage == 0);

    // キー入力待ち
    waiting = true;
    while (waiting) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_KEY_DOWN || e.type == SDL_EVENT_QUIT) {
                waiting = false;
                break;
            }
        }
        SDL_Delay(16);
    }

    // ダメージ適用
    if (result.damage > 0) {
        defender->takeDamage(result.damage);
    }

    // 6) 反撃処理（将来実装）
    if (result.isCountered) {
        std::cout << "[BattleScene] Counter attack!\n";
    }

    // 7) 戦闘結果表示
    battleUI.showBattleResult(result, attacker->getId(), defender->getId());

    // キー入力待ち
    waiting = true;
    while (waiting) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_KEY_DOWN || e.type == SDL_EVENT_QUIT) {
                waiting = false;
                break;
            }
        }
        SDL_Delay(16);
    }

    return result;
}

void BattleScene::showBattlePreview(
    const Unit* attacker,
    const Unit* defender,
    const WeaponData* weapon,
    int hitRate)
{
    std::stringstream ss;
    ss << "=== 戦闘開始 ===\n";
    ss << attacker->getId() << " → " << defender->getId() << "\n";
    ss << "命中率: " << hitRate << "%\n";

    if (weapon) {
        ss << "武器: " << weapon->name << "\n";
    }

    ss << "\nHP: " << attacker->getHp() << " vs " << defender->getHp();

    ui_->showMessage(ss.str());
}

void BattleScene::playAttackAnimation(
    const Unit* attacker,
    const WeaponData* weapon)
{
    std::stringstream ss;
    ss << attacker->getId() << " の攻撃！";

    ui_->showMessage(ss.str());
}

void BattleScene::showDamage(
    const Unit* target,
    int damage,
    bool isCritical)
{
    std::stringstream ss;

    if (isCritical) {
        ss << "★ クリティカルヒット！ ★\n";
    }

    ss << target->getId() << " に " << damage << " のダメージ！\n";

    int hpAfter = target->getHp() - damage;
    if (hpAfter < 0) hpAfter = 0;

    ss << "HP: " << target->getHp() << " → " << hpAfter;

    ui_->showMessage(ss.str());
}

void BattleScene::showBattleResult(
    const BattleResult& result,
    const Unit* attacker,
    const Unit* defender)
{
    std::stringstream ss;
    ss << "=== 戦闘結果 ===\n";

    if (result.defenderDestroyed) {
        ss << defender->getId() << " を撃破！\n";
    }

    ss << "経験値: +" << result.expGained << "\n";

    if (result.isCountered) {
        ss << "\n反撃ダメージ: " << result.counterDamage << "\n";
        if (result.attackerDestroyed) {
            ss << attacker->getId() << " が撃破された！\n";
        }
    }

    ui_->showMessage(ss.str());
}