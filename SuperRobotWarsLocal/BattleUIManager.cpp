#include "BattleUIManager.h"
#include "UIManager.h"
#include "Unit.h"
#include <SDL.h>
#include <iostream>  // ← 追加
#include <sstream>
#include <iomanip>

BattleUIManager::BattleUIManager(SDLRenderer* renderer, UIManager* uiManager)
    : renderer_(renderer)
    , uiManager_(uiManager)
    , demoAttacker_(nullptr)
    , demoDefender_(nullptr)
    , animationFrame_(0)
    , isAnimating_(false)
    , font_(nullptr)
    , texMgr_(nullptr)  // ← 追加
{
    // フォント読み込み
    font_ = TTF_OpenFont("assets/mplus-1m-regular.ttf", 24);
    if (!font_) {
        std::cerr << "[BattleUIManager] Failed to load font: "
            << SDL_GetError() << "\n";
    }
}

BattleUIManager::~BattleUIManager() {
    if (font_) {
        TTF_CloseFont(font_);
    }
}

void BattleUIManager::showBattlePreview(
    const Unit* attacker,
    const Unit* defender,
    int hitRate,
    int critRate)
{
    if (!attacker || !defender) return;

    auto* sdlRen = renderer_->getSDLRenderer();

    // 画面クリア
    SDL_SetRenderDrawColor(sdlRen, 0, 0, 0, 255);
    renderer_->clear();

    const int windowW = 640;
    const int windowH = 480;
    const int boxW = 280;
    const int boxH = 180;
    const int margin = 20;

    // 攻撃側情報（左側）
    int leftX = margin;
    int topY = windowH / 2 - boxH / 2;

    drawBox(leftX, topY, boxW, boxH, true);

    // 攻撃側の名前
    std::stringstream ss;
    ss << attacker->getId();
    drawTextCentered(ss.str(), leftX, topY + 10, boxW);

    // HP表示
    ss.str("");
    ss << "HP: " << attacker->getHp() << "/" << attacker->getMaxHp();
    drawTextCentered(ss.str(), leftX, topY + 40, boxW);

    // HPバー
    float hpRatio = static_cast<float>(attacker->getHp()) / attacker->getMaxHp();
    drawBar(leftX + 20, topY + 70, boxW - 40, 20, hpRatio, 0, 255, 0);

    // 防御側情報（右側）
    int rightX = windowW - boxW - margin;

    drawBox(rightX, topY, boxW, boxH, true);

    // 防御側の名前
    ss.str("");
    ss << defender->getId();
    drawTextCentered(ss.str(), rightX, topY + 10, boxW);

    // HP表示
    ss.str("");
    ss << "HP: " << defender->getHp() << "/" << defender->getMaxHp();
    drawTextCentered(ss.str(), rightX, topY + 40, boxW);

    // HPバー
    hpRatio = static_cast<float>(defender->getHp()) / defender->getMaxHp();
    drawBar(rightX + 20, topY + 70, boxW - 40, 20, hpRatio, 0, 255, 0);

    // 中央に戦闘情報
    int centerY = topY + boxH + 40;

    ss.str("");
    ss << "命中率: " << hitRate << "%";
    drawTextCentered(ss.str(), 0, centerY, windowW);

    ss.str("");
    ss << "クリティカル率: " << critRate << "%";
    drawTextCentered(ss.str(), 0, centerY + 30, windowW);

    renderer_->present();
}

void BattleUIManager::showDamage(int damage, bool isCritical, bool isMiss)
{
    auto* sdlRen = renderer_->getSDLRenderer();

    // まず戦闘デモ画面を描画
    renderBattleDemo();

    const int windowW = 640;
    const int windowH = 480;

    // 半透明の黒背景（赤ではなく）
    SDL_SetRenderDrawBlendMode(sdlRen, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(sdlRen, 0, 0, 0, 200);  // 黒に変更
    SDL_FRect bgRect{
        windowW / 4.0f,
        windowH / 2.0f - 80,
        windowW / 2.0f,
        160
    };
    SDL_RenderFillRect(sdlRen, &bgRect);

    // 枠線（白）
    SDL_SetRenderDrawColor(sdlRen, 255, 255, 255, 255);
    SDL_RenderRect(sdlRen, &bgRect);

    std::stringstream ss;

    if (isMiss) {
        ss << "MISS!";
        drawTextCentered(ss.str(), 0, windowH / 2 - 40, windowW);
    }
    else {
        if (isCritical) {
            ss << "CRITICAL HIT!";
            drawTextCentered(ss.str(), 0, windowH / 2 - 50, windowW);
        }
        else {
            ss << "HIT!";
            drawTextCentered(ss.str(), 0, windowH / 2 - 40, windowW);
        }

        ss.str("");
        ss << damage << " Damage";
        drawTextCentered(ss.str(), 0, windowH / 2 + 10, windowW);
    }

    renderer_->present();
}

void BattleUIManager::showBattleResult(
    const BattleResult& result,
    const std::string& attackerName,
    const std::string& defenderName)
{
    auto* sdlRen = renderer_->getSDLRenderer();

    // 画面クリア
    SDL_SetRenderDrawColor(sdlRen, 0, 0, 0, 255);
    renderer_->clear();

    const int windowW = 640;
    const int windowH = 480;
    const int boxW = 400;
    const int boxH = 300;
    const int boxX = (windowW - boxW) / 2;
    const int boxY = (windowH - boxH) / 2;

    // 結果ボックス
    drawBox(boxX, boxY, boxW, boxH, true);

    // タイトル
    drawTextCentered("=== Battle Result ===", boxX, boxY + 20, boxW);

    int textY = boxY + 60;
    const int lineHeight = 30;

    // 攻撃側情報
    std::stringstream ss;
    ss << attackerName;
    drawTextCentered(ss.str(), boxX, textY, boxW);
    textY += lineHeight;

    // ダメージ情報
    ss.str("");
    ss << "Damage: " << result.damage;
    if (result.isCritical) {
        ss << " (CRITICAL!)";
    }
    drawTextCentered(ss.str(), boxX, textY, boxW);
    textY += lineHeight;

    // 経験値
    ss.str("");
    ss << "EXP: +" << result.expGained;
    drawTextCentered(ss.str(), boxX, textY, boxW);
    textY += lineHeight;

    // 撃破判定
    if (result.defenderDestroyed) {
        textY += 10;
        drawTextCentered("*** DESTROYED ***", boxX, textY, boxW);
    }

    // 反撃情報（将来実装）
    if (result.isCountered) {
        textY += lineHeight + 10;
        ss.str("");
        ss << "Counter Damage: " << result.counterDamage;
        drawTextCentered(ss.str(), boxX, textY, boxW);
    }

    renderer_->present();
}

void BattleUIManager::initBattleDemo(const Unit* attacker, const Unit* defender)
{
    demoAttacker_ = attacker;
    demoDefender_ = defender;
    animationFrame_ = 0;
    isAnimating_ = true;
}

void BattleUIManager::renderBattleDemo()
{
    if (!demoAttacker_ || !demoDefender_) return;

    auto* sdlRen = renderer_->getSDLRenderer();

    // 画面クリア（濃い青背景）
    SDL_SetRenderDrawColor(sdlRen, 20, 20, 60, 255);
    renderer_->clear();

    const int windowW = 640;
    const int windowH = 480;
    const int halfH = windowH / 2;

    // 上半分の背景（攻撃側エリア）- やや明るい青
    SDL_FRect topArea{ 0, 0, static_cast<float>(windowW), static_cast<float>(halfH) };
    SDL_SetRenderDrawColor(sdlRen, 40, 60, 100, 255);
    SDL_RenderFillRect(sdlRen, &topArea);

    // 下半分の背景（防御側エリア）- やや暗い赤
    SDL_FRect bottomArea{ 0, static_cast<float>(halfH),
                          static_cast<float>(windowW),
                          static_cast<float>(halfH) };
    SDL_SetRenderDrawColor(sdlRen, 80, 40, 40, 255);
    SDL_RenderFillRect(sdlRen, &bottomArea);

    // 中央の区切り線（太く）
    SDL_SetRenderDrawColor(sdlRen, 255, 255, 255, 255);
    SDL_RenderLine(sdlRen, 0, halfH - 2, windowW, halfH - 2);
    SDL_RenderLine(sdlRen, 0, halfH - 1, windowW, halfH - 1);
    SDL_RenderLine(sdlRen, 0, halfH, windowW, halfH);
    SDL_RenderLine(sdlRen, 0, halfH + 1, windowW, halfH + 1);

    // 上半分：攻撃側ユニット表示エリア（修正：はみ出さないように）
    SDL_FRect attackerArea{ 180, 90, 280, 150 };  // y位置とサイズ調整
    SDL_SetRenderDrawColor(sdlRen, 100, 150, 255, 255);
    SDL_RenderFillRect(sdlRen, &attackerArea);
    SDL_SetRenderDrawColor(sdlRen, 255, 255, 255, 255);
    SDL_RenderRect(sdlRen, &attackerArea);

    // 下半分：防御側ユニット表示エリア（修正：はみ出さないように）
    SDL_FRect defenderArea{ 180, halfH + 90, 280, 150 };  // y位置とサイズ調整
    SDL_SetRenderDrawColor(sdlRen, 255, 100, 100, 255);
    SDL_RenderFillRect(sdlRen, &defenderArea);
    SDL_SetRenderDrawColor(sdlRen, 255, 255, 255, 255);
    SDL_RenderRect(sdlRen, &defenderArea);

    // ユニット名表示
    drawTextCentered(demoAttacker_->getId(), 0, 30, windowW);
    drawTextCentered(demoDefender_->getId(), 0, halfH + 30, windowW);

    // HP表示
    std::stringstream ss;
    ss << "HP: " << demoAttacker_->getHp() << "/" << demoAttacker_->getMaxHp();
    drawTextCentered(ss.str(), 0, 60, windowW);

    ss.str("");
    ss << "HP: " << demoDefender_->getHp() << "/" << demoDefender_->getMaxHp();
    drawTextCentered(ss.str(), 0, halfH + 60, windowW);

    renderer_->present();
}

void BattleUIManager::playAttackAnimation(bool isAttacker)
{
    // TODO: 実際の攻撃アニメーション
    // - スプライトの移動
    // - エフェクトの表示
    // - カメラシェイク
    animationFrame_++;
}

void BattleUIManager::playDamageEffect(int x, int y, int damage)
{
    // TODO: ダメージエフェクト
    // - 数値の飛び出し
    // - 爆発エフェクト
    // - 画面フラッシュ
}

// プライベートヘルパー関数

void BattleUIManager::drawBox(int x, int y, int w, int h, bool filled)
{
    auto* sdlRen = renderer_->getSDLRenderer();
    SDL_FRect rect{
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(w),
        static_cast<float>(h)
    };

    SDL_SetRenderDrawBlendMode(sdlRen, SDL_BLENDMODE_BLEND);

    if (filled) {
        SDL_SetRenderDrawColor(sdlRen, 40, 40, 80, 200);
        SDL_RenderFillRect(sdlRen, &rect);
    }

    // 枠線
    SDL_SetRenderDrawColor(sdlRen, 255, 255, 255, 255);
    SDL_RenderRect(sdlRen, &rect);
}

void BattleUIManager::drawTextCentered(
    const std::string& text,
    int x,
    int y,
    int w)
{
    if (!font_ || text.empty()) return;

    auto* sdlRen = renderer_->getSDLRenderer();

    // テキストをレンダリング
    SDL_Color color{ 255, 255, 255, 255 };
    SDL_Surface* surf = TTF_RenderText_Blended(font_, text.c_str(), 0, color);
    if (!surf) {
        std::cerr << "[BattleUIManager] Text render failed: "
            << SDL_GetError() << "\n";
        return;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(sdlRen, surf);
    if (!tex) {
        std::cerr << "[BattleUIManager] Texture creation failed: "
            << SDL_GetError() << "\n";
        SDL_DestroySurface(surf);
        return;
    }

    // 中央揃えで描画
    int textW = surf->w;
    int textH = surf->h;
    int centerX = x + (w - textW) / 2;

    SDL_FRect srcRect{ 0, 0,
                       static_cast<float>(textW),
                       static_cast<float>(textH) };
    SDL_FRect dstRect{ static_cast<float>(centerX),
                       static_cast<float>(y),
                       static_cast<float>(textW),
                       static_cast<float>(textH) };

    SDL_RenderTexture(sdlRen, tex, &srcRect, &dstRect);

    SDL_DestroyTexture(tex);
    SDL_DestroySurface(surf);
}

void BattleUIManager::drawBar(
    int x,
    int y,
    int w,
    int h,
    float ratio,
    Uint8 r,
    Uint8 g,
    Uint8 b)
{
    auto* sdlRen = renderer_->getSDLRenderer();

    // 背景（黒）
    SDL_FRect bgRect{
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(w),
        static_cast<float>(h)
    };
    SDL_SetRenderDrawColor(sdlRen, 40, 40, 40, 255);
    SDL_RenderFillRect(sdlRen, &bgRect);

    // バー本体
    int barW = static_cast<int>(w * ratio);
    SDL_FRect barRect{
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(barW),
        static_cast<float>(h)
    };
    SDL_SetRenderDrawColor(sdlRen, r, g, b, 255);
    SDL_RenderFillRect(sdlRen, &barRect);

    // 枠線
    SDL_SetRenderDrawColor(sdlRen, 255, 255, 255, 255);
    SDL_RenderRect(sdlRen, &bgRect);
}