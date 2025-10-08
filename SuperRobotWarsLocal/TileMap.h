#pragma once

#include <string>
#include "SDLRenderer.h"

/// @brief BMP限定のタイルマップ読み込み・描画クラス
class TileMap {
public:
    TileMap(SDLRenderer* renderer, int tileWidth, int tileHeight);
    ~TileMap();

    /// @brief BMPファイルから読み込む
    bool loadFromFile(const std::string& mapPath);

    void render(int offsetX, int offsetY) const;

    int getMapWidth()  const { return mapWidth_; }
    int getMapHeight() const { return mapHeight_; }
    int getTileWidth() const { return tileWidth_; }
    int getTileHeight() const { return tileHeight_; }

private:
    SDLRenderer* renderer_;
    SDL_Texture* texture_;
    int          mapWidth_;
    int          mapHeight_;
    int          tileWidth_;
    int          tileHeight_;
};