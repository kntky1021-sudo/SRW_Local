#pragma once
#include <string>
#include <vector>

class SDLRenderer;
struct SDL_Texture;

class TileMap {
public:
    explicit TileMap(SDLRenderer* renderer);
    ~TileMap();

    bool loadFromFile(const std::string& jsonPath,
        const std::string& tilesetBmpPath,
        int tileWidth,
        int tileHeight);

    /// @param offsetX,offsetY カメラオフセット(pixel)
    void render(int offsetX, int offsetY) const;

    // マップ・タイルサイズ取得
    int getMapWidth()  const { return mapW_; }
    int getMapHeight() const { return mapH_; }
    int getTileWidth() const { return tileW_; }
    int getTileHeight() const { return tileH_; }

private:
    SDLRenderer* renderer_;
    SDL_Texture* tilesetTex_;
    int               mapW_, mapH_;
    int               tileW_, tileH_;
    std::vector<int>  tiles_;
};