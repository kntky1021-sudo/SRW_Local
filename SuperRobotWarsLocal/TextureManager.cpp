#include "TextureManager.h"
#include "SDLRenderer.h"
#include <iostream>

TextureManager::TextureManager(SDLRenderer* renderer)
    : renderer_(renderer)
{
}

TextureManager::~TextureManager() {
    clearAll();
}

bool TextureManager::loadTexture(const std::string& id, const std::string& filepath) {
    // 既に読み込み済みなら何もしない
    if (textureInfos_.find(id) != textureInfos_.end()) {
        std::cout << "[TextureManager] Texture already loaded: " << id << "\n";
        return true;
    }

    // BMP画像を読み込み
    SDL_Surface* surface = SDL_LoadBMP(filepath.c_str());
    if (!surface) {
        std::cerr << "[TextureManager] Failed to load BMP: " << filepath
            << " - " << SDL_GetError() << "\n";
        return false;
    }

    // テクスチャ作成
    SDL_Texture* texture = SDL_CreateTextureFromSurface(
        renderer_->getSDLRenderer(), surface);

    if (!texture) {
        std::cerr << "[TextureManager] Failed to create texture: "
            << SDL_GetError() << "\n";
        SDL_DestroySurface(surface);
        return false;
    }

    // サイズを記録
    TextureInfo info;
    info.texture = texture;
    info.width = surface->w;
    info.height = surface->h;

    textureInfos_[id] = info;
    textures_[id] = texture;

    SDL_DestroySurface(surface);

    std::cout << "[TextureManager] Loaded texture: " << id
        << " (" << info.width << "x" << info.height << ")\n";

    return true;
}

bool TextureManager::loadTexturePNG(const std::string& id, const std::string& filepath) {
    // TODO: SDL_imageを使ったPNG読み込み
    // 現状はBMPのみ対応
    std::cerr << "[TextureManager] PNG loading not implemented yet\n";
    return false;
}

void TextureManager::drawTexture(const std::string& id, int x, int y, int w, int h) {
    auto it = textureInfos_.find(id);
    if (it == textureInfos_.end()) {
        std::cerr << "[TextureManager] Texture not found: " << id << "\n";
        return;
    }

    const TextureInfo& info = it->second;

    // サイズ指定がなければ元サイズ
    if (w == 0) w = info.width;
    if (h == 0) h = info.height;

    SDL_FRect srcRect{ 0, 0,
                       static_cast<float>(info.width),
                       static_cast<float>(info.height) };
    SDL_FRect dstRect{ static_cast<float>(x),
                       static_cast<float>(y),
                       static_cast<float>(w),
                       static_cast<float>(h) };

    SDL_RenderTexture(renderer_->getSDLRenderer(),
        info.texture, &srcRect, &dstRect);
}

void TextureManager::drawTextureEx(
    const std::string& id,
    int srcX, int srcY, int srcW, int srcH,
    int dstX, int dstY, int dstW, int dstH)
{
    auto it = textureInfos_.find(id);
    if (it == textureInfos_.end()) {
        std::cerr << "[TextureManager] Texture not found: " << id << "\n";
        return;
    }

    SDL_FRect srcRect{ static_cast<float>(srcX),
                       static_cast<float>(srcY),
                       static_cast<float>(srcW),
                       static_cast<float>(srcH) };
    SDL_FRect dstRect{ static_cast<float>(dstX),
                       static_cast<float>(dstY),
                       static_cast<float>(dstW),
                       static_cast<float>(dstH) };

    SDL_RenderTexture(renderer_->getSDLRenderer(),
        it->second.texture, &srcRect, &dstRect);
}

bool TextureManager::getTextureSize(const std::string& id, int& w, int& h) {
    auto it = textureInfos_.find(id);
    if (it == textureInfos_.end()) {
        return false;
    }

    w = it->second.width;
    h = it->second.height;
    return true;
}

void TextureManager::unloadTexture(const std::string& id) {
    auto it = textureInfos_.find(id);
    if (it != textureInfos_.end()) {
        SDL_DestroyTexture(it->second.texture);
        textureInfos_.erase(it);
        textures_.erase(id);
        std::cout << "[TextureManager] Unloaded texture: " << id << "\n";
    }
}

void TextureManager::clearAll() {
    for (auto& pair : textureInfos_) {
        SDL_DestroyTexture(pair.second.texture);
    }
    textureInfos_.clear();
    textures_.clear();
    std::cout << "[TextureManager] All textures cleared\n";
}