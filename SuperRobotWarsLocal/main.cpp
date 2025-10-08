#include <SDL.h>
#include <iostream>
#include "SDLRenderer.h"
#include "UIManager.h"
#include "InputManager.h"
#include "GameManager.h"

int main(int argc, char** argv) {
    // SDL 本体初期化（SDL3 は false が失敗）
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return -1;
    }

    constexpr int WINDOW_W = 640;
    constexpr int WINDOW_H = 480;

    // ウィンドウ作成
    SDL_Window* window = SDL_CreateWindow(
        "Super Robot Wars - Prototype",
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

    // 各種マネージャ生成
    UIManager uiManager(&sdlRenderer);
    InputManager inputManager;

    // ゲームマネージャー作成
    GameManager gameManager(
        &uiManager,
        &inputManager,
        &sdlRenderer,
        WINDOW_W,
        WINDOW_H
    );

    // ゲーム実行
    gameManager.run();

    // 後始末
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}