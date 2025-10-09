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
    std::cout << "[BattleManager] Battle initialized\n";
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
    for (const auto& u : units_) {
        if (u.hp <= 0) continue;  // 撃破済みはスキップ

        SDL_FRect frect{
            float(u.x * 32 - offsetX),
            float(u.y * 32 - offsetY),
            32.0f,
            32.0f
        };

        SDL_SetRenderDrawBlendMode(sdlR, SDL_BLENDMODE_BLEND);

        // 敵味方で色分け
        if (u.isEnemy) {
            // 敵：赤色
            SDL_SetRenderDrawColor(sdlR, 255, 0, 0, 128);
        }
        else {
            // 味方：青色
            SDL_SetRenderDrawColor(sdlR, 0, 0, 255, 128);
        }

        SDL_RenderFillRect(sdlR, &frect);

        // 枠線
        if (u.isEnemy) {
            SDL_SetRenderDrawColor(sdlR, 255, 0, 0, 255);
        }
        else {
            SDL_SetRenderDrawColor(sdlR, 0, 0, 255, 255);
        }
        SDL_RenderRect(sdlR, &frect);

        // 行動済みの場合は暗く表示
        if (u.hasActed) {
            SDL_SetRenderDrawColor(sdlR, 0, 0, 0, 100);
            SDL_RenderFillRect(sdlR, &frect);
        }
    }
}

// 指定座標にいるユニットIDを返す。いなければ -1。
int BattleManager::getUnitAt(int x, int y) const {
    for (size_t i = 0; i < units_.size(); ++i) {
        if (units_[i].x == x && units_[i].y == y && units_[i].hp > 0) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// ダメージ計算の stub 実装
int BattleManager::calculateDamage(int attackerId, int defenderId) const {
    (void)attackerId;
    (void)defenderId;
    return 10;  // 仮ダメージ
}

// ダメージ適用の stub 実装
void BattleManager::applyDamage(int unitId, int damage) {
    if (unitId >= 0 && unitId < static_cast<int>(units_.size())) {
        units_[unitId].hp -= damage;
        if (units_[unitId].hp < 0) {
            units_[unitId].hp = 0;
        }
        std::cout << "[BattleManager] Unit " << units_[unitId].name
            << " took " << damage << " damage. HP: "
            << units_[unitId].hp << "\n";
    }
}

void BattleManager::attack(int attackerX, int attackerY, int defenderX, int defenderY) {
    int attackerId = getUnitAt(attackerX, attackerY);
    int defenderId = getUnitAt(defenderX, defenderY);

    if (attackerId < 0) {
        std::cerr << "[BattleManager] No attacker at (" << attackerX << ", " << attackerY << ")\n";
        return;
    }

    if (defenderId < 0) {
        std::cerr << "[BattleManager] No defender at (" << defenderX << ", " << defenderY << ")\n";
        return;
    }

    std::cout << "[BattleManager] Battle: Unit " << attackerId
        << " vs Unit " << defenderId << "\n";

    // 実際の戦闘システムを使用する場合は、
    // UnitDatabaseから取得したユニット情報を使用
    // 今は簡易版として既存のダメージ計算を使用

    int damage = calculateDamage(attackerId, defenderId);
    applyDamage(defenderId, damage);

    std::cout << "[BattleManager] Unit " << units_[attackerId].name
        << " attacked Unit " << units_[defenderId].name
        << " for " << damage << " damage\n";

    // 撃破時の処理
    if (units_[defenderId].hp <= 0) {
        std::cout << "[BattleManager] Unit " << units_[defenderId].name
            << " has been destroyed!\n";
        // TODO: 経験値付与
        // TODO: 撃破カウント増加
    }
}

// ユニット追加
int BattleManager::addUnit(int x, int y, bool isEnemy, const std::string& name) {
    Unit unit;
    unit.x = x;
    unit.y = y;
    unit.hp = isEnemy ? 10 : 100;  // 敵はHP10、味方はHP100
    unit.isEnemy = isEnemy;
    unit.hasActed = false;
    unit.name = name;

    units_.push_back(unit);
    int id = static_cast<int>(units_.size()) - 1;

    std::cout << "[BattleManager] Unit added: " << name
        << " (ID:" << id << ") at (" << x << ", " << y << ") "
        << (isEnemy ? "Enemy" : "Ally")
        << " HP:" << unit.hp << "\n";

    return id;
}

// 行動済みフラグ管理
void BattleManager::setUnitActed(int unitId, bool acted) {
    if (unitId >= 0 && unitId < static_cast<int>(units_.size())) {
        units_[unitId].hasActed = acted;
    }
}

bool BattleManager::hasUnitActed(int unitId) const {
    if (unitId >= 0 && unitId < static_cast<int>(units_.size())) {
        return units_[unitId].hasActed;
    }
    return false;
}

void BattleManager::resetAllActedFlags(bool enemyOnly) {
    for (auto& unit : units_) {
        if (!enemyOnly || unit.isEnemy) {
            unit.hasActed = false;
        }
    }
    std::cout << "[BattleManager] Acted flags reset"
        << (enemyOnly ? " (enemies only)" : "") << "\n";
}

// ユニット情報取得
bool BattleManager::isEnemyUnit(int unitId) const {
    if (unitId >= 0 && unitId < static_cast<int>(units_.size())) {
        return units_[unitId].isEnemy;
    }
    return false;
}

std::string BattleManager::getUnitName(int unitId) const {
    if (unitId >= 0 && unitId < static_cast<int>(units_.size())) {
        return units_[unitId].name;
    }
    return "Unknown";
}

// 生存数取得
int BattleManager::getAliveAllyCount() const {
    int count = 0;
    for (const auto& unit : units_) {
        if (!unit.isEnemy && unit.hp > 0) {
            count++;
        }
    }
    return count;
}

int BattleManager::getAliveEnemyCount() const {
    int count = 0;
    for (const auto& unit : units_) {
        if (unit.isEnemy && unit.hp > 0) {
            count++;
        }
    }
    return count;
}


// BattleManager.cpp の末尾に以下のメソッドを追加：

// ユニット情報取得の拡張
int BattleManager::getUnitHP(int unitId) const {
    if (unitId >= 0 && unitId < static_cast<int>(units_.size())) {
        return units_[unitId].hp;
    }
    return 0;
}

int BattleManager::getUnitMaxHP(int unitId) const {
    if (unitId >= 0 && unitId < static_cast<int>(units_.size())) {
        // 現状は固定値、後でロボットデータから取得
        return units_[unitId].isEnemy ? 100 : 1000;
    }
    return 0;
}

// ユニットの経験値・レベル管理
void BattleManager::addExperience(int unitId, int exp) {
    if (unitId >= 0 && unitId < static_cast<int>(units_.size())) {
        // TODO: 経験値システムの実装
        std::cout << "[BattleManager] Unit " << unitId
            << " gained " << exp << " exp\n";
    }
}

bool BattleManager::checkLevelUp(int unitId) {
    // TODO: レベルアップ判定
    return false;
}