#include <SDL.h>           // SDL3（メイン）
#include <SDL_mixer.h>    // SDL2_mixer（音響のみ）
#include <iostream>
#include "SDLRenderer.h"
#include "UIManager.h"
#include "InputManager.h"
#include "GameManager.h"
#include "AudioManager.h"

int main(int argc, char** argv) {

    // SDL3 初期化（映像・入力）
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL3_Init failed: " << SDL_GetError() << "\n";
        return -1;
    }

    // SDL2 音声サブシステム初期化（SDL2_mixer用）
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL_InitSubSystem(AUDIO) failed: " << SDL_GetError() << "\n";
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

    // AudioManager初期化（SDL2_mixer使用）
    AudioManager audioManager;
    if (!audioManager.initialize()) {
        std::cerr << "[Main] Warning: AudioManager initialization failed\n";
        std::cerr << "[Main] Game will continue without sound\n";
    }
    else {
        // BGM読み込み
        audioManager.loadBGM("title", "assets/audio/bgm/title.wav");
        audioManager.loadBGM("battle", "assets/audio/bgm/battle.wav");
        audioManager.loadBGM("map", "assets/audio/bgm/map.wav");

        // 効果音プリロード
        audioManager.preloadCommonSE();

        // マスター音量設定
        audioManager.setMasterVolume(80);

        // タイトルBGM再生
        audioManager.playBGM("title", true, 1000);
    }

    GameManager gameManager(
        &uiManager,
        &inputManager,
        &sdlRenderer,
        WINDOW_W,
        WINDOW_H
    );

    // GameManagerにAudioManagerを渡す
    gameManager.setAudioManager(&audioManager);

    std::cout << "[Main] Starting game loop...\n";
    gameManager.run();

    // 終了処理
    std::cout << "[Main] Shutting down...\n";
    audioManager.shutdown();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}