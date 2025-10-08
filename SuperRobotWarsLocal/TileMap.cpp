#include "TileMap.h"
#include <SDL.h>
#include <iostream>

TileMap::TileMap(SDLRenderer* renderer, int tileWidth, int tileHeight)
    : renderer_(renderer)
    , texture_(nullptr)
    , mapWidth_(0)
    , mapHeight_(0)
    , tileWidth_(tileWidth)
    , tileHeight_(tileHeight)
{
}

TileMap::~TileMap() {
    if (texture_) {
        SDL_DestroyTexture(texture_);
    }
}

bool TileMap::loadFromFile(const std::string& mapPath) {
    SDL_Surface* surf = SDL_LoadBMP(mapPath.c_str());
    if (!surf) {
        std::cerr << "[TileMap] SDL_LoadBMP failed: " << SDL_GetError() << "\n";
        return false;
    }

    auto ren = renderer_->getSDLRenderer();
    texture_ = SDL_CreateTextureFromSurface(ren, surf);
    SDL_DestroySurface(surf);

    if (!texture_) {
        std::cerr << "[TileMap] CreateTexture failed: " << SDL_GetError() << "\n";
        return false;
    }

    // SDL3: テクスチャサイズは float* で取得
    float w, h;
    if (!SDL_GetTextureSize(texture_, &w, &h)) {
        std::cerr << "[TileMap] SDL_GetTextureSize failed: " << SDL_GetError() << "\n";
        return false;
    }
    mapWidth_ = static_cast<int>(w) / tileWidth_;
    mapHeight_ = static_cast<int>(h) / tileHeight_;

    std::cout << "[TileMap] loaded " << mapPath
        << " (" << mapWidth_ << "x" << mapHeight_ << " tiles)\n";
    return true;
}

void TileMap::render(int offsetX, int offsetY) const {
    if (!texture_) return;

    auto ren = renderer_->getSDLRenderer();
    for (int y = 0; y < mapHeight_; ++y) {
        for (int x = 0; x < mapWidth_; ++x) {
            // 整数矩形を浮動小数点に変換
            SDL_FRect srcF{
                static_cast<float>(x * tileWidth_),
                static_cast<float>(y * tileHeight_),
                static_cast<float>(tileWidth_),
                static_cast<float>(tileHeight_)
            };
            SDL_FRect dstF{
                static_cast<float>(x * tileWidth_ - offsetX),
                static_cast<float>(y * tileHeight_ - offsetY),
                static_cast<float>(tileWidth_),
                static_cast<float>(tileHeight_)
            };
            // SDL3 では SDL_RenderTexture() を呼ぶ
            SDL_RenderTexture(ren, texture_, &srcF, &dstF);
        }
    }
}