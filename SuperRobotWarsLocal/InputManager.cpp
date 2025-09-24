#include "InputManager.h"

InputManager::InputManager()
    : quitRequested_(false)
    , lastChoice_(-1)
{
}

void InputManager::pollEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            quitRequested_ = true;
        }
    }
}

bool InputManager::quitRequested() const {
    return quitRequested_;
}

void InputManager::waitKey() {
    SDL_Event e;
    while (SDL_WaitEvent(&e)) {
        if (e.type == SDL_EVENT_KEY_DOWN) {
            return;
        }
    }
}

int InputManager::waitForChoice(int maxChoices) {
    SDL_Event e;
    while (SDL_WaitEvent(&e)) {
        if (e.type == SDL_EVENT_KEY_DOWN) {
            SDL_Keycode key = e.key.key;
            // 上段キー対応
            if (key >= SDLK_0 && key <= SDLK_9) {
                int idx = key - SDLK_0;
                if (idx < maxChoices) return idx;
            }
            // テンキー対応
            if (key >= SDLK_KP_0 && key <= SDLK_KP_9) {
                int idx = key - SDLK_KP_0;
                if (idx < maxChoices) return idx;
            }
        }
    }
    return 0;
}