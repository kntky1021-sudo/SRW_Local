#include "Cursor.h"
#include "SDLRenderer.h"
#include <SDL.h>

Cursor::Cursor(SDLRenderer* renderer, int tileW, int tileH)
    : renderer_(renderer)
    , tileW_(tileW)
    , tileH_(tileH)
    , x_(0)
    , y_(0)
{
}

void Cursor::setPosition(int tx, int ty) {
    x_ = tx;
    y_ = ty;
}

void Cursor::move(int dx, int dy, int mapW, int mapH) {
    x_ = SDL_clamp(x_ + dx, 0, mapW - 1);
    y_ = SDL_clamp(y_ + dy, 0, mapH - 1);
}

void Cursor::render(int offsetX, int offsetY) const {
    auto* sdlR = renderer_->getSDLRenderer();

    SDL_FRect frect{
        float(x_ * tileW_ - offsetX),
        float(y_ * tileH_ - offsetY),
        float(tileW_),
        float(tileH_)
    };

    SDL_SetRenderDrawBlendMode(sdlR, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(sdlR, 255, 255, 0, 128);
    SDL_RenderFillRect(sdlR, &frect);

    SDL_SetRenderDrawColor(sdlR, 255, 255, 0, 255);
    SDL_RenderRect(sdlR, &frect);
}