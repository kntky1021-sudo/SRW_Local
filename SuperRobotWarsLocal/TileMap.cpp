#include "TileMap.h"
#include "SDLRenderer.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <SDL.h>
#include <iostream>

TileMap::TileMap(SDLRenderer* renderer)
    : renderer_(renderer)
    , tilesetTex_(nullptr)
    , mapW_(0)
    , mapH_(0)
    , tileW_(0)
    , tileH_(0)
{
}

TileMap::~TileMap() {
    if (tilesetTex_) {
        SDL_DestroyTexture(tilesetTex_);
    }
}

bool TileMap::loadFromFile(const std::string& jsonPath,
    const std::string& tilesetBmpPath,
    int tileWidth,
    int tileHeight)
{
    std::ifstream ifs(jsonPath);
    if (!ifs) {
        std::cerr << "[TileMap] cannot open " << jsonPath << "\n";
        return false;
    }

    nlohmann::json j;
    try {
        ifs >> j;
    }
    catch (const nlohmann::json::exception& e) {
        std::cerr << "[TileMap] JSON parse error: " << e.what() << "\n";
        return false;
    }

    mapW_ = j.value("width", 0);
    mapH_ = j.value("height", 0);
    tileW_ = tileWidth;
    tileH_ = tileHeight;

    auto arr = j.value("tiles", nlohmann::json::array());
    tiles_.clear();
    tiles_.reserve(mapW_ * mapH_);
    for (auto& v : arr) {
        tiles_.push_back(v.get<int>());
    }

    SDL_Surface* surf = SDL_LoadBMP(tilesetBmpPath.c_str());
    if (!surf) {
        std::cerr << "[TileMap] SDL_LoadBMP failed: " << SDL_GetError() << "\n";
        return false;
    }

    tilesetTex_ = SDL_CreateTextureFromSurface(
        renderer_->getSDLRenderer(), surf);
    SDL_DestroySurface(surf);
    if (!tilesetTex_) {
        std::cerr << "[TileMap] CreateTexture failed: " << SDL_GetError() << "\n";
        return false;
    }

    return true;
}

void TileMap::render(int offsetX, int offsetY) const {
    if (!tilesetTex_) return;

    float fTexW = 0.0f, fTexH = 0.0f;
    if (!SDL_GetTextureSize(tilesetTex_, &fTexW, &fTexH)) {
        std::cerr << "[TileMap] SDL_GetTextureSize failed: " << SDL_GetError() << "\n";
        return;
    }

    int texW = static_cast<int>(fTexW);
    int tilesPerRow = texW / tileW_;

    for (int y = 0; y < mapH_; ++y) {
        for (int x = 0; x < mapW_; ++x) {
            int idx = tiles_[y * mapW_ + x];
            if (idx < 0) continue;

            int tx = (idx % tilesPerRow) * tileW_;
            int ty = (idx / tilesPerRow) * tileH_;

            SDL_FRect src{
                static_cast<float>(tx),
                static_cast<float>(ty),
                static_cast<float>(tileW_),
                static_cast<float>(tileH_)
            };
            SDL_FRect dst{
                static_cast<float>(x * tileW_ - offsetX),
                static_cast<float>(y * tileH_ - offsetY),
                static_cast<float>(tileW_),
                static_cast<float>(tileH_)
            };

            renderer_->drawTexture(tilesetTex_, src, dst);
        }
    }
}