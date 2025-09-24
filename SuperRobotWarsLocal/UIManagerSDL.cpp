#include "UIManager.h"
#include <SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>

namespace {
    constexpr float MARGIN_X = 15.0f;
    constexpr float MARGIN_Y = 15.0f;
}

UIManager::UIManager(SDLRenderer* renderer)
    : renderer_(renderer)
    , font_(nullptr)
{
    if (!TTF_Init()) {
        std::cerr << "TTF_Init failed: " << SDL_GetError() << "\n";
        return;
    }
    font_ = TTF_OpenFont("assets/mplus-1m-regular.ttf", 24);
    if (!font_) {
        std::cerr << "TTF_OpenFont failed: " << SDL_GetError() << "\n";
    }
}

UIManager::~UIManager() {
    if (font_) {
        TTF_CloseFont(font_);
    }
    TTF_Quit();
}

void UIManager::showMessage(const std::string& text) {
    if (!font_) {
        std::cout << text << "\n";
        return;
    }

    SDL_Color color{ 255, 255, 255, 255 };
    // UTF-8 対応済みのテキスト描画関数
    SDL_Surface* surf = TTF_RenderText_Blended(font_, text.c_str(), 0, color);
    if (!surf) {
        std::cerr << "TTF_RenderText_Blended failed: " << SDL_GetError() << "\n";
        return;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(
        renderer_->getSDLRenderer(), surf);
    if (!tex) {
        std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << "\n";
        SDL_DestroySurface(surf);
        return;
    }

    // src/dst それぞれの矩形を明示的に作成
    SDL_FRect src{ 0.0f, 0.0f,
                  static_cast<float>(surf->w),
                  static_cast<float>(surf->h) };
    SDL_FRect dst{ MARGIN_X, MARGIN_Y,
                  src.w, src.h };

    SDL_DestroySurface(surf);

    renderer_->clear();
    renderer_->drawTexture(tex, src, dst);
    renderer_->present();

    SDL_DestroyTexture(tex);
}

void UIManager::promptChoice(const std::vector<std::string>& options) {
    if (!font_) {
        for (size_t i = 0; i < options.size(); ++i) {
            std::cout << "[" << i << "] " << options[i] << "\n";
        }
        return;
    }

    renderer_->clear();
    SDL_Color color{ 255, 255, 255, 255 };
    float y = MARGIN_Y;

    for (size_t i = 0; i < options.size(); ++i) {
        std::string line = "[" + std::to_string(i) + "] " + options[i];
        SDL_Surface* surf = TTF_RenderText_Blended(font_, line.c_str(), 0, color);
        if (!surf) {
            std::cerr << "TTF_RenderText_Blended failed: " << SDL_GetError() << "\n";
            continue;
        }

        SDL_Texture* tex = SDL_CreateTextureFromSurface(
            renderer_->getSDLRenderer(), surf);
        if (!tex) {
            std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << "\n";
            SDL_DestroySurface(surf);
            continue;
        }

        SDL_FRect src{ 0.0f, 0.0f,
                      static_cast<float>(surf->w),
                      static_cast<float>(surf->h) };
        SDL_FRect dst{ MARGIN_X, y,
                      src.w, src.h };

        SDL_DestroySurface(surf);

        renderer_->drawTexture(tex, src, dst);
        SDL_DestroyTexture(tex);

        y += dst.h + 5.0f;
    }

    renderer_->present();
}