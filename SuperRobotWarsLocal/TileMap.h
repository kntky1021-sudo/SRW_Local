#pragma once

#include <string>
#include <SDL.h>

/// @brief タイルマップの読み込み・描画を担うクラス
class TileMap {
public:
    /// コンストラクタ：SDL_Renderer とタイルサイズを渡す
    TileMap(SDL_Renderer* renderer, int tileWidth, int tileHeight);
    ~TileMap();

    /// @brief ファイルからタイルセット画像を読み込む
    /// @return 成功時 true
    bool loadFromFile(const std::string& mapPath);

    /// @brief 指定パスから読み込む (旧 load 互換)
    bool load(const std::string& mapPath) { return loadFromFile(mapPath); }

    /// @brief 指定マスの移動コストを返す（未実装時は常に1を返す）
    int getCost(int mapX, int mapY) const;

    void render(int offsetX, int offsetY) const;
    int  getMapWidth()  const { return mapWidth_; }
    int  getMapHeight() const { return mapHeight_; }
    int  getTileWidth() const { return tileWidth_; }
    int  getTileHeight() const { return tileHeight_; }

private:
    SDL_Renderer* renderer_;
    SDL_Texture* texture_;
    int           mapWidth_;
    int           mapHeight_;
    int           tileWidth_;
    int           tileHeight_;
    int           textureWidth_;
    int           textureHeight_;
};