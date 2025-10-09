#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <SDL.h>

class SDLRenderer;

/// テクスチャ管理クラス
/// 画像ファイルの読み込み・キャッシュ・描画を管理
class TextureManager {
public:
    explicit TextureManager(SDLRenderer* renderer);
    ~TextureManager();

    /// 画像を読み込む（BMPのみ対応）
    /// @param id テクスチャID（後で参照するための識別子）
    /// @param filepath 画像ファイルパス
    /// @return 成功したらtrue
    bool loadTexture(const std::string& id, const std::string& filepath);

    /// PNG画像を読み込む（将来実装）
    /// @param id テクスチャID
    /// @param filepath 画像ファイルパス
    /// @return 成功したらtrue
    bool loadTexturePNG(const std::string& id, const std::string& filepath);

    /// テクスチャを描画
    /// @param id テクスチャID
    /// @param x 描画先X座標
    /// @param y 描画先Y座標
    /// @param w 描画幅（0なら元サイズ）
    /// @param h 描画高さ（0なら元サイズ）
    void drawTexture(const std::string& id, int x, int y,
        int w = 0, int h = 0);

    /// テクスチャの一部を描画（スプライトシート用）
    /// @param id テクスチャID
    /// @param srcX ソース矩形X
    /// @param srcY ソース矩形Y
    /// @param srcW ソース矩形幅
    /// @param srcH ソース矩形高さ
    /// @param dstX 描画先X
    /// @param dstY 描画先Y
    /// @param dstW 描画幅
    /// @param dstH 描画高さ
    void drawTextureEx(const std::string& id,
        int srcX, int srcY, int srcW, int srcH,
        int dstX, int dstY, int dstW, int dstH);

    /// テクスチャのサイズを取得
    /// @param id テクスチャID
    /// @param[out] w 幅
    /// @param[out] h 高さ
    /// @return 成功したらtrue
    bool getTextureSize(const std::string& id, int& w, int& h);

    /// テクスチャを解放
    /// @param id テクスチャID
    void unloadTexture(const std::string& id);

    /// すべてのテクスチャを解放
    void clearAll();

private:
    SDLRenderer* renderer_;
    std::unordered_map<std::string, SDL_Texture*> textures_;

    // テクスチャの元サイズを記録
    struct TextureInfo {
        SDL_Texture* texture;
        int width;
        int height;
    };
    std::unordered_map<std::string, TextureInfo> textureInfos_;
};