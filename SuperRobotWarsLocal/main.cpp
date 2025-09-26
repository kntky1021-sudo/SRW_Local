#include <SDL.h>
#include <iostream>
#include "UIManager.h"
#include "InputManager.h"
#include "TileMap.h"
#include "Cursor.h"
#include "BattleManager.h"
#include "ExecutionEngine.h"

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: "
            << SDL_GetError() << "\n";
        return 1;
    }

    constexpr int WINDOW_W = 640;
    constexpr int WINDOW_H = 480;
    constexpr int TILE_W = 32;
    constexpr int TILE_H = 32;

    SDL_Window* window = SDL_CreateWindow(
        "SRPG Prototype",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_W,
        WINDOW_H,
        0
    );
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: "
            << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED |
        SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: "
            << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // TileMap の構築＆読み込み
    TileMap tileMap(renderer, TILE_W, TILE_H);
    if (!tileMap.loadFromFile("maps/tileset.bmp")) {
        std::cerr << "Failed to load tileset\n";
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    UIManager     uiManager(renderer);
    InputManager  inputManager;
    Cursor        cursor;
    BattleManager battleManager;

    ExecutionEngine engine(
        &uiManager,
        &inputManager,
        &tileMap,
        &cursor,
        &battleManager,
        WINDOW_W,
        WINDOW_H
    );

    // スクリプト実行
    engine.run("scripts/sample_script.json");

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
