#include "BattleManager.h"
#include "BattleCalculator.h"
#include <SDL.h>
#include <iostream>

BattleManager::BattleManager(SDLRenderer* renderer)
    : renderer_(renderer)
{
}

BattleManager::~BattleManager() = default;

void BattleManager::startBattle(const std::string& /*mapName*/) {
    units_.clear();
    unitMap_.clear();
    actedFlags_.clear();
    std::cout << "[BattleManager] Battle initialized\n";
}

void BattleManager::registerUnit(std::unique_ptr<Unit> unit, int x, int y) {
    if (!unit) {
        std::cerr << "[BattleManager] Null unit provided\n";
        return;
    }

    // 座標設定
    unit->setPosition(x, y);

    // ID取得
    std::string unitId = unit->getId();

    // 登録
    Unit* unitPtr = unit.get();
    units_.push_back(std::move(unit));
    unitMap_[unitId] = unitPtr;
    actedFlags_[unitId] = false;

    std::cout << "[BattleManager] Unit registered: " << unitPtr->getId()
        << " at (" << x << ", " << y << ") "
        << (unitPtr->getTeam() == Team::Ally ? "Ally" : "Enemy")
        << " HP:" << unitPtr->getHp() << "/" << unitPtr->getMaxHp() << "\n";
}

Unit* BattleManager::getUnitById(const std::string& unitId) {
    auto it = unitMap_.find(unitId);
    if (it != unitMap_.end()) {
        return it->second;
    }
    return nullptr;
}

Unit* BattleManager::getUnitAt(int x, int y) {
    for (auto& unit : units_) {
        if (unit->getX() == x && unit->getY() == y && unit->isAlive()) {
            return unit.get();
        }
    }
    return nullptr;
}

const Unit* BattleManager::getUnitAt(int x, int y) const {
    for (const auto& unit : units_) {
        if (unit->getX() == x && unit->getY() == y && unit->isAlive()) {
            return unit.get();
        }
    }
    return nullptr;
}

void BattleManager::moveUnit(const std::string& unitId, int x, int y) {
    Unit* unit = getUnitById(unitId);
    if (unit) {
        unit->setPosition(x, y);
        std::cout << "[BattleManager] Unit " << unitId
            << " moved to (" << x << ", " << y << ")\n";
    }
    else {
        std::cerr << "[BattleManager] Unit not found: " << unitId << "\n";
    }
}

void BattleManager::attack(const std::string& attackerId, const std::string& defenderId) {
    Unit* attacker = getUnitById(attackerId);
    Unit* defender = getUnitById(defenderId);

    if (!attacker || !defender) {
        std::cerr << "[BattleManager] Invalid attack: attacker or defender not found\n";
        return;
    }

    std::cout << "[BattleManager] Battle: " << attacker->getId()
        << " vs " << defender->getId() << "\n";

    // TODO: 武器選択ロジック（現在は最初の武器を使用）
    const WeaponData* weapon = attacker->getWeapon(0);

    // 戦闘計算実行
    BattleResult result = BattleCalculator::execute(
        attacker,
        defender,
        nullptr,  // 武器は後で実装
        'L'       // 地形
    );

    // ダメージ適用
    if (result.damage > 0) {
        defender->takeDamage(result.damage);
        std::cout << "[BattleManager] " << defender->getId()
            << " took " << result.damage << " damage. HP: "
            << defender->getHp() << "\n";
    }

    // 経験値付与
    addExperience(attackerId, result.expGained);

    // 撃破判定
    if (result.defenderDestroyed) {
        std::cout << "[BattleManager] " << defender->getId()
            << " has been destroyed!\n";
    }
}

void BattleManager::renderUnits(SDLRenderer* renderer, int offsetX, int offsetY) const {
    auto* sdlR = renderer->getSDLRenderer();

    for (const auto& unit : units_) {
        if (!unit->isAlive()) continue;

        SDL_FRect frect{
            static_cast<float>(unit->getX() * 32 - offsetX),
            static_cast<float>(unit->getY() * 32 - offsetY),
            32.0f,
            32.0f
        };

        SDL_SetRenderDrawBlendMode(sdlR, SDL_BLENDMODE_BLEND);

        // 敵味方で色分け
        if (unit->getTeam() == Team::Enemy) {
            SDL_SetRenderDrawColor(sdlR, 255, 0, 0, 128);
        }
        else {
            SDL_SetRenderDrawColor(sdlR, 0, 0, 255, 128);
        }

        SDL_RenderFillRect(sdlR, &frect);

        // 枠線
        if (unit->getTeam() == Team::Enemy) {
            SDL_SetRenderDrawColor(sdlR, 255, 0, 0, 255);
        }
        else {
            SDL_SetRenderDrawColor(sdlR, 0, 0, 255, 255);
        }
        SDL_RenderRect(sdlR, &frect);

        // 行動済みの場合は暗く表示
        if (hasUnitActed(unit->getId())) {
            SDL_SetRenderDrawColor(sdlR, 0, 0, 0, 100);
            SDL_RenderFillRect(sdlR, &frect);
        }
    }
}

void BattleManager::setUnitActed(const std::string& unitId, bool acted) {
    actedFlags_[unitId] = acted;
}

bool BattleManager::hasUnitActed(const std::string& unitId) const {
    auto it = actedFlags_.find(unitId);
    if (it != actedFlags_.end()) {
        return it->second;
    }
    return false;
}

void BattleManager::resetAllActedFlags(bool enemyOnly) {
    for (auto& pair : actedFlags_) {
        if (enemyOnly) {
            Unit* unit = getUnitById(pair.first);
            if (unit && unit->getTeam() == Team::Enemy) {
                pair.second = false;
            }
        }
        else {
            pair.second = false;
        }
    }
    std::cout << "[BattleManager] Acted flags reset"
        << (enemyOnly ? " (enemies only)" : "") << "\n";
}

bool BattleManager::isEnemyUnit(const std::string& unitId) const {
    const Unit* unit = findUnitById(unitId);
    return unit && unit->getTeam() == Team::Enemy;
}

std::string BattleManager::getUnitName(const std::string& unitId) const {
    const Unit* unit = findUnitById(unitId);
    return unit ? unit->getId() : "Unknown";
}

int BattleManager::getAliveAllyCount() const {
    int count = 0;
    for (const auto& unit : units_) {
        if (unit->getTeam() == Team::Ally && unit->isAlive()) {
            count++;
        }
    }
    return count;
}

int BattleManager::getAliveEnemyCount() const {
    int count = 0;
    for (const auto& unit : units_) {
        if (unit->getTeam() == Team::Enemy && unit->isAlive()) {
            count++;
        }
    }
    return count;
}

void BattleManager::addExperience(const std::string& unitId, int exp) {
    Unit* unit = getUnitById(unitId);
    if (unit) {
        std::cout << "[BattleManager] Unit " << unitId
            << " gained " << exp << " exp\n";
        // TODO: 経験値システムの完全実装
    }
}

bool BattleManager::checkLevelUp(const std::string& unitId) {
    // TODO: レベルアップ判定
    return false;
}

std::vector<std::string> BattleManager::getAllUnitIds() const {
    std::vector<std::string> ids;
    ids.reserve(units_.size());
    for (const auto& unit : units_) {
        ids.push_back(unit->getId());
    }
    return ids;
}

std::vector<std::string> BattleManager::getAllyUnitIds() const {
    std::vector<std::string> ids;
    for (const auto& unit : units_) {
        if (unit->getTeam() == Team::Ally) {
            ids.push_back(unit->getId());
        }
    }
    return ids;
}

std::vector<std::string> BattleManager::getEnemyUnitIds() const {
    std::vector<std::string> ids;
    for (const auto& unit : units_) {
        if (unit->getTeam() == Team::Enemy) {
            ids.push_back(unit->getId());
        }
    }
    return ids;
}

void BattleManager::printAllUnits() const {
    std::cout << "\n[BattleManager] === All Units ===\n";
    for (const auto& unit : units_) {
        std::cout << "  " << unit->getId()
            << " (" << (unit->getTeam() == Team::Ally ? "Ally" : "Enemy") << ")"
            << " at (" << unit->getX() << ", " << unit->getY() << ")"
            << " HP:" << unit->getHp() << "/" << unit->getMaxHp()
            << (unit->isAlive() ? "" : " [DESTROYED]")
            << "\n";
    }
    std::cout << "[BattleManager] Total: " << units_.size() << " units\n\n";
}

Unit* BattleManager::findUnitById(const std::string& unitId) {
    auto it = unitMap_.find(unitId);
    return (it != unitMap_.end()) ? it->second : nullptr;
}

const Unit* BattleManager::findUnitById(const std::string& unitId) const {
    auto it = unitMap_.find(unitId);
    return (it != unitMap_.end()) ? it->second : nullptr;
}