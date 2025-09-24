#pragma once

#include <string>
#include <vector>
#include "SDLRenderer.h"
#include <SDL3_ttf/SDL_ttf.h>

class UIManager {
public:
    explicit UIManager(SDLRenderer* renderer);
    ~UIManager();
    void showMessage(const std::string& text);
    void promptChoice(const std::vector<std::string>& options);

    // 追加：BattleStartCommand などで直接描画したいときに使う
    SDLRenderer* getRenderer() const { return renderer_; }

private:
    SDLRenderer* renderer_;
    TTF_Font* font_;
};
