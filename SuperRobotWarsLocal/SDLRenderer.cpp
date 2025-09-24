#include "SDLRenderer.h"
#include <iostream>

SDLRenderer::SDLRenderer(SDL_Renderer* renderer)
    : renderer_(renderer)
{
}

SDLRenderer::~SDLRenderer() {
    // SDL_DestroyRenderer は main 側で呼び出すためここでは何もしない
}

void SDLRenderer::clear() {
    SDL_RenderClear(renderer_);
}

void SDLRenderer::present() {
    SDL_RenderPresent(renderer_);
}

void SDLRenderer::drawTexture(SDL_Texture* tex,
    const SDL_FRect& src,
    const SDL_FRect& dst)
{
    if (!SDL_RenderTexture(renderer_, tex, &src, &dst)) {
        std::cerr << "SDL_RenderTexture failed: "
            << SDL_GetError() << "\n";
    }
}