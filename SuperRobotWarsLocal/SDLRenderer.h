#pragma once

#include <SDL.h>

/// SDL_Renderer* をラップする軽量クラス
class SDLRenderer {
public:
    /// SDL_CreateRenderer で得た SDL_Renderer* を渡す
    explicit SDLRenderer(SDL_Renderer* renderer);
    ~SDLRenderer();

    /// 画面クリア → present
    void clear();
    void present();

    /// 浮動小数点対応テクスチャ描画
    ///   src, dst は SDL_FRect
    void drawTexture(SDL_Texture* tex,
        const SDL_FRect& src,
        const SDL_FRect& dst);

    SDL_Renderer* getSDLRenderer() const { return renderer_; }

private:
    SDL_Renderer* renderer_;
};