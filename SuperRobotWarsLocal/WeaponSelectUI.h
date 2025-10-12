#pragma once

#include <vector>
#include <string>
#include "UnitData.h"
#include <SDL3_ttf/SDL_ttf.h>

class SDLRenderer;
class UIManager;

/// 武器選択UI
/// 複数の武器から攻撃に使用する武器を選択
class WeaponSelectUI {
public:
    WeaponSelectUI(SDLRenderer* renderer, UIManager* uiManager);
    ~WeaponSelectUI();

    /// 武器選択メニューを表示し、選択された武器のインデックスを返す
    /// @param weapons 武器リスト
    /// @param currentEN 現在のEN
    /// @param targetDistance 対象までの距離
    /// @return 選択された武器のインデックス（キャンセル時は-1）
    int selectWeapon(
        const std::vector<WeaponData>& weapons,
        int currentEN,
        int targetDistance);

    /// 武器情報を表示
    /// @param weapon 武器データ
    /// @param index 選択中のインデックス
    /// @param isSelected 選択中か
    void drawWeaponInfo(
        const WeaponData& weapon,
        int index,
        bool isSelected);

private:
    SDLRenderer* renderer_;
    UIManager* uiManager_;

    // 描画ヘルパー
    void drawWeaponList(
        const std::vector<WeaponData>& weapons,
        int selectedIndex,
        int currentEN,
        int targetDistance);

    void drawBox(int x, int y, int w, int h);
    void drawText(const std::string& text, int x, int y);

    // フォント付きテキスト描画（新規追加）
    void drawTextWithFont(
        TTF_Font* font,
        const std::string& text,
        int x,
        int y,
        SDL_Color color);

    // 武器が使用可能かチェック
    bool isWeaponUsable(
        const WeaponData& weapon,
        int currentEN,
        int targetDistance) const;
};