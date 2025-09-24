#include "BattleManager.h"
#include <SDL.h>
#include <iostream>

BattleManager::BattleManager(SDLRenderer* renderer)
    : renderer_(renderer)
{
}

BattleManager::~BattleManager() = default;

void BattleManager::startBattle(const std::string& /*mapName*/) {
    units_.clear();
    units_.push_back({ 0, 0 });
}

void BattleManager::moveUnit(int unitId, int x, int y) {
    if (unitId >= 0 && unitId < static_cast<int>(units_.size())) {
        units_[unitId].x = x;
        units_[unitId].y = y;
    }
}

std::array<int, 2> BattleManager::getUnitPosition(int unitId) const {
    if (unitId >= 0 && unitId < static_cast<int>(units_.size())) {
        return { units_[unitId].x, units_[unitId].y };
    }
    return { 0, 0 };
}

void BattleManager::renderUnits(SDLRenderer* renderer,
    int offsetX,
    int offsetY) const {
    auto* sdlR = renderer->getSDLRenderer();
    for (auto& u : units_) {
        SDL_FRect frect{
            float(u.x * 32 - offsetX),
            float(u.y * 32 - offsetY),
            32.0f,
            32.0f
        };

        SDL_SetRenderDrawBlendMode(sdlR, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(sdlR, 255, 0, 0, 128);
        SDL_RenderFillRect(sdlR, &frect);

        SDL_SetRenderDrawColor(sdlR, 255, 0, 0, 255);
        SDL_RenderRect(sdlR, &frect);
    }
}