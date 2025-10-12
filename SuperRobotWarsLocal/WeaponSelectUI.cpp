#include "WeaponSelectUI.h"
#include "SDLRenderer.h"
#include "UIManager.h"
#include <SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>
#include <sstream>

WeaponSelectUI::WeaponSelectUI(SDLRenderer* renderer, UIManager* uiManager)
    : renderer_(renderer)
    , uiManager_(uiManager)
{
}

WeaponSelectUI::~WeaponSelectUI() = default;

int WeaponSelectUI::selectWeapon(
    const std::vector<WeaponData>& weapons,
    int currentEN,
    int targetDistance)
{
    if (weapons.empty()) {
        std::cerr << "[WeaponSelectUI] No weapons available\n";
        return -1;
    }

    int selectedIndex = 0;
    bool decided = false;
    bool cancelled = false;

    // 最初の使用可能な武器を選択
    for (size_t i = 0; i < weapons.size(); ++i) {
        if (isWeaponUsable(weapons[i], currentEN, targetDistance)) {
            selectedIndex = static_cast<int>(i);
            break;
        }
    }

    while (!decided && !cancelled) {
        // 武器リスト描画
        drawWeaponList(weapons, selectedIndex, currentEN, targetDistance);

        // 入力処理
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                cancelled = true;
                break;
            }
            else if (e.type == SDL_EVENT_KEY_DOWN) {
                switch (e.key.scancode) {
                case SDL_SCANCODE_UP:
                    selectedIndex = (selectedIndex - 1 + static_cast<int>(weapons.size()))
                        % static_cast<int>(weapons.size());
                    break;

                case SDL_SCANCODE_DOWN:
                    selectedIndex = (selectedIndex + 1) % static_cast<int>(weapons.size());
                    break;

                case SDL_SCANCODE_Z:
                case SDL_SCANCODE_RETURN:
                    // 使用可能な武器のみ選択可能
                    if (isWeaponUsable(weapons[selectedIndex], currentEN, targetDistance)) {
                        decided = true;
                    }
                    break;

                case SDL_SCANCODE_X:
                case SDL_SCANCODE_ESCAPE:
                    cancelled = true;
                    break;

                default:
                    break;
                }
            }
        }

        SDL_Delay(16);
    }

    return cancelled ? -1 : selectedIndex;
}

void WeaponSelectUI::drawWeaponList(
    const std::vector<WeaponData>& weapons,
    int selectedIndex,
    int currentEN,
    int targetDistance)
{
    auto* sdlRen = renderer_->getSDLRenderer();

    // 画面クリア
    SDL_SetRenderDrawColor(sdlRen, 0, 0, 40, 255);
    renderer_->clear();

    const int windowW = 640;
    const int windowH = 480;
    const int boxW = 500;
    const int boxH = 400;
    const int boxX = (windowW - boxW) / 2;
    const int boxY = (windowH - boxH) / 2;

    // 背景ボックス
    drawBox(boxX, boxY, boxW, boxH);

    // タイトル
    drawText("=== Select Weapon ===", boxX + 10, boxY + 10);

    // フォント読み込み
    TTF_Font* font = TTF_OpenFont("assets/mplus-1m-regular.ttf", 20);
    if (!font) {
        std::cerr << "[WeaponSelectUI] Failed to load font\n";
        renderer_->present();
        return;
    }

    // 武器リスト
    int yOffset = boxY + 50;
    const int lineHeight = 60;

    for (size_t i = 0; i < weapons.size(); ++i) {
        const WeaponData& weapon = weapons[i];
        bool isSelected = (static_cast<int>(i) == selectedIndex);
        bool usable = isWeaponUsable(weapon, currentEN, targetDistance);

        // 選択カーソル
        if (isSelected) {
            SDL_FRect cursorRect{
                static_cast<float>(boxX + 15),
                static_cast<float>(yOffset + 5),
                10, 10
            };
            SDL_SetRenderDrawColor(sdlRen, 255, 255, 0, 255);
            SDL_RenderFillRect(sdlRen, &cursorRect);
        }

        // 武器名
        std::stringstream ss;
        ss << weapon.name;
        if (!usable) {
            ss << " (----)";
        }

        SDL_Color textColor = usable ?
            SDL_Color{ 255, 255, 255, 255 } :
            SDL_Color{ 128, 128, 128, 255 };

        drawTextWithFont(font, ss.str(), boxX + 40, yOffset, textColor);

        // 武器情報
        ss.str("");
        ss << "Power:" << weapon.power
            << " Range:" << weapon.minRange << "-" << weapon.maxRange;
        drawTextWithFont(font, ss.str(), boxX + 40, yOffset + 22,
            SDL_Color{ 200, 200, 200, 255 });

        // EN/弾数
        ss.str("");
        if (weapon.enCost > 0) {
            ss << "EN:" << weapon.enCost;
        }
        if (weapon.ammo >= 0) {
            if (weapon.enCost > 0) ss << " ";
            ss << "Ammo:" << weapon.ammo;
        }
        drawTextWithFont(font, ss.str(), boxX + 40, yOffset + 42,
            SDL_Color{ 180, 180, 180, 255 });

        yOffset += lineHeight;
    }

    // 操作説明
    drawTextWithFont(font, "UP/DOWN: Select  Z: Decide  X: Cancel",
        boxX + 10, boxY + boxH - 30, SDL_Color{ 255, 255, 100, 255 });

    TTF_CloseFont(font);

    renderer_->present();
}

void WeaponSelectUI::drawWeaponInfo(
    const WeaponData& weapon,
    int index,
    bool isSelected)
{
    // 個別の武器情報描画（将来拡張用）
}

void WeaponSelectUI::drawBox(int x, int y, int w, int h)
{
    auto* sdlRen = renderer_->getSDLRenderer();

    // 背景
    SDL_FRect bgRect{
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(w),
        static_cast<float>(h)
    };
    SDL_SetRenderDrawBlendMode(sdlRen, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(sdlRen, 40, 40, 80, 220);
    SDL_RenderFillRect(sdlRen, &bgRect);

    // 枠線
    SDL_SetRenderDrawColor(sdlRen, 255, 255, 255, 255);
    SDL_RenderRect(sdlRen, &bgRect);
}

void WeaponSelectUI::drawText(const std::string& text, int x, int y)
{
    // 簡易版（マーカーのみ）
    auto* sdlRen = renderer_->getSDLRenderer();
    SDL_SetRenderDrawColor(sdlRen, 255, 255, 255, 255);
    SDL_RenderPoint(sdlRen, x, y);
}

void WeaponSelectUI::drawTextWithFont(
    TTF_Font* font,
    const std::string& text,
    int x,
    int y,
    SDL_Color color)
{
    if (!font || text.empty()) return;

    auto* sdlRen = renderer_->getSDLRenderer();

    SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), 0, color);
    if (!surf) return;

    SDL_Texture* tex = SDL_CreateTextureFromSurface(sdlRen, surf);
    if (!tex) {
        SDL_DestroySurface(surf);
        return;
    }

    SDL_FRect srcRect{ 0, 0,
                       static_cast<float>(surf->w),
                       static_cast<float>(surf->h) };
    SDL_FRect dstRect{ static_cast<float>(x),
                       static_cast<float>(y),
                       static_cast<float>(surf->w),
                       static_cast<float>(surf->h) };

    SDL_RenderTexture(sdlRen, tex, &srcRect, &dstRect);

    SDL_DestroyTexture(tex);
    SDL_DestroySurface(surf);
}

bool WeaponSelectUI::isWeaponUsable(
    const WeaponData& weapon,
    int currentEN,
    int targetDistance) const
{
    // EN不足チェック
    if (weapon.enCost > currentEN) {
        return false;
    }

    // 弾数チェック
    if (weapon.ammo == 0) {
        return false;
    }

    // 射程チェック
    if (targetDistance < weapon.minRange || targetDistance > weapon.maxRange) {
        return false;
    }

    return true;
}