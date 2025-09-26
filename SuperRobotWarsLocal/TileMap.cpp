#include "TileMap.h"
#include <SDL.h>
#include <iostream>

TileMap::TileMap(SDL_Renderer* renderer, int tileWidth, int tileHeight)
    : renderer_(renderer)
    , texture_(nullptr)
    , mapWidth_(0)
    , mapHeight_(0)
    , tileWidth_(tileWidth)
    , tileHeight_(tileHeight)
    , textureWidth_(0)
    , textureHeight_(0)
{
}

TileMap::~TileMap() {
    if (texture_) {
        SDL_DestroyTexture(texture_);
    }
}

bool TileMap::loadFromFile(const std::string& mapPath) {
    // BMP 形式のファイルを読み込む例（後で IMG_Load などに置き換えても OK）
    SDL_Surface* surf = SDL_LoadBMP(mapPath.c_str());
    if (!surf) {
        std::cerr << "[TileMap] SDL_LoadBMP failed: "
            << SDL_GetError() << "\n";
        return false;
    }
    texture_ = SDL_CreateTextureFromSurface(renderer_, surf);
    SDL_FreeSurface(surf);
    if (!texture_) {
        std::cerr << "[TileMap] CreateTexture failed: "
            << SDL_GetError() << "\n";
        return false;
    }

    // テクスチャ全体サイズを取得
    SDL_QueryTexture(texture_, nullptr, nullptr,
        &textureWidth_, &textureHeight_);

    // マップの「タイル数」を計算
    mapWidth_ = textureWidth_ / tileWidth_;
    mapHeight_ = textureHeight_ / tileHeight_;

    std::cout << "[TileMap] loaded " << mapPath
        << " (" << mapWidth_ << "x" << mapHeight_
        << " tiles)\n";
    return true;
}

int TileMap::getCost(int mapX, int mapY) const {
    // TODO: タイル種別に応じたコスト返却ロジックに差し替え
    // 今は全マス cost=1 固定
    (void)mapX; (void)mapY;
    return 1;
}

void TileMap::render(int offsetX, int offsetY) const {
    if (!texture_) return;

    SDL_Rect src, dst;
    for (int ty = 0; ty < mapHeight_; ++ty) {
        for (int tx = 0; tx < mapWidth_; ++tx) {
            src.x = tx * tileWidth_;
            src.y = ty * tileHeight_;
            src.w = tileWidth_;
            src.h = tileHeight_;

            dst.x = tx * tileWidth_ - offsetX;
            dst.y = ty * tileHeight_ - offsetY;
            dst.w = tileWidth_;
            dst.h = tileHeight_;

            SDL_RenderCopy(renderer_, texture_, &src, &dst);
        }
    }
}