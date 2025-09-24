#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <iostream>
#include "SDLRenderer.h"
#include "UIManager.h"
#include "InputManager.h"
#include "TileMap.h"
#include "Cursor.h"
#include "BattleManager.h"
#include "ExecutionEngine.h"

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Super Robot Wars Local",
        800, 600, 0);
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* rawR = SDL_CreateRenderer(window, nullptr);
    if (!rawR) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDLRenderer   renderer(rawR);
    UIManager     ui(&renderer);
    InputManager  input;
    constexpr int TILE_W = 32, TILE_H = 32;

    TileMap       tileMap(&renderer);
    tileMap.loadFromFile("maps/map01.json", "maps/tileset.bmp", TILE_W, TILE_H);

    Cursor        cursor(&renderer, TILE_W, TILE_H);
    BattleManager battleManager(&renderer);

    // ウィンドウサイズ (800x600) を渡す
    ExecutionEngine engine(&ui,
        &input,
        &tileMap,
        &cursor,
        &battleManager,
        800, 600);
    engine.run("scripts/sample_script.json");

    SDL_DestroyRenderer(rawR);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}