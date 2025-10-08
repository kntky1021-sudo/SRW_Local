#include <SDL.h>
#include <iostream>
#include <filesystem>           // C++17 の filesystem
#include "SDLRenderer.h"
#include "UIManager.h"
#include "InputManager.h"
#include "TileMap.h"
#include "Cursor.h"
#include "BattleManager.h"
#include "ExecutionEngine.h"

int main(int argc, char** argv) {
    // SDL 本体初期化（SDL3 は false が失敗）
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return -1;
    }

    constexpr int WINDOW_W = 640;
    constexpr int WINDOW_H = 480;
    constexpr int TILE_W = 32;
    constexpr int TILE_H = 32;

    // ウィンドウ作成
    SDL_Window* window = SDL_CreateWindow(
        "SRPG Prototype",
        WINDOW_W,
        WINDOW_H,
        SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }

    // レンダラー作成
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // SDLRenderer ラッパー
    SDLRenderer sdlRenderer(renderer);

    // TileMap 読み込み(BMP限定)
    TileMap tileMap(&sdlRenderer, TILE_W, TILE_H);
    if (!tileMap.loadFromFile("maps/tileset.bmp")) {
        std::cerr << "[TileMap] loadFromFile failed\n";
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // 各種マネージャ生成
    UIManager    uiManager(&sdlRenderer);
    InputManager inputManager;
    Cursor       cursor(&sdlRenderer, TILE_W, TILE_H);
    BattleManager battleManager(&sdlRenderer);

    // エンジン組み立て
    ExecutionEngine engine(
        &uiManager,
        &inputManager,
        &tileMap,
        &cursor,
        &battleManager,
        WINDOW_W,
        WINDOW_H
    );

    // ← ここからデバッグログ追加
    const std::string scriptPath = "scripts/sample_script.json";

    // カレントディレクトリを出力
    std::cout << "CWD = "
        << std::filesystem::current_path().string()
        << "\n";

    // スクリプト存在チェック
    if (!std::filesystem::exists(scriptPath)) {
        std::cerr << "[Error] Script not found: "
            << scriptPath << "\n";
    }
    else {
        std::cout << "[Info] Found script: "
            << scriptPath << "\n";
    }
    // ← ここまで

    // スクリプト実行
    engine.run(scriptPath);

    // 後始末
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}