#include <SDL.h>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <filesystem>
#include "SDLRenderer.h"
#include "UIManager.h"
#include "InputManager.h"
#include "GameManager.h"
#include "UnitDatabase.h"

int main(int argc, char** argv) {

    // SDL 本体初期化
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return -1;
    }

    constexpr int WINDOW_W = 640;
    constexpr int WINDOW_H = 480;

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

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDLRenderer sdlRenderer(renderer);
    UIManager uiManager(&sdlRenderer);
    InputManager inputManager;

    // ユニットデータベース初期化
    UnitDatabase unitDB;
    std::cout << "[Main] Loading unit database...\n";

    if (!unitDB.loadRobots("data/robots.json")) {
        std::cerr << "[Main] Warning: Failed to load robots.json\n";
    }

    if (!unitDB.loadPilots("data/pilots.json")) {
        std::cerr << "[Main] Warning: Failed to load pilots.json\n";
    }

    auto robotIds = unitDB.getRobotIds();
    std::cout << "[Main] Loaded " << robotIds.size() << " robots:\n";
    for (const auto& id : robotIds) {
        auto* robot = unitDB.getRobot(id);
        if (robot) {
            std::cout << "  - " << robot->name << " (" << id << ")\n";
        }
    }

    auto pilotIds = unitDB.getPilotIds();
    std::cout << "[Main] Loaded " << pilotIds.size() << " pilots:\n";
    for (const auto& id : pilotIds) {
        auto* pilot = unitDB.getPilot(id);
        if (pilot) {
            std::cout << "  - " << pilot->name << " (" << id << ")\n";
        }
    }

    GameManager gameManager(
        &uiManager,
        &inputManager,
        &sdlRenderer,
        WINDOW_W,
        WINDOW_H
    );

    gameManager.run();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}