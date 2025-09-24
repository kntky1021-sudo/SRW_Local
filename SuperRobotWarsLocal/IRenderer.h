#pragma once

#include <vector>
#include <string>

// タイル描画用
struct TileSprite {
    int x;          // マップ上の列
    int y;          // マップ上の行
    int textureId;  // 今後テクスチャ管理で使う想定（現状は未使用）
};

// ユニット描画用
struct UnitSprite {
    int x;           // マップ上の列
    int y;           // マップ上の行
    std::string id;  // 今後テクスチャファイル名などに使う想定
};

class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual void drawMap(const std::vector<std::string>& ascii) = 0;
    virtual void drawTiles(const std::vector<TileSprite>& tiles) = 0;
    virtual void drawUnits(const std::vector<UnitSprite>& units) = 0;
    virtual void drawHUD(const std::vector<std::string>& lines) = 0;
    virtual void present() = 0;
};