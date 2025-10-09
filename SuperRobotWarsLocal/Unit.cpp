#include "Unit.h"

// コンストラクタ
Unit::Unit(std::string id_,
    Team team_,
    char symbol_,
    int maxHp_,
    int moveRange_,
    int attackRange_,
    int attackPower_,
    int defensePower_,
    int speed_)
    : id(std::move(id_))
    , team(team_)
    , symbol(symbol_)
    , hp(maxHp_)
    , maxHp(maxHp_)
    , moveRange(moveRange_)
    , attackRange(attackRange_)
    , attackPower(attackPower_)
    , defensePower(defensePower_)
    , speed(speed_)
    , x(0)
    , y(0)
    , currentEN_(100)  // 追加
    , maxEN_(100)      // 追加
{
}

// 座標取得／設定
int Unit::getX() const { return x; }
int Unit::getY() const { return y; }
void Unit::setPosition(int x_, int y_) { x = x_; y = y_; }

// 基本ステータス取得
const std::string& Unit::getId() const { return id; }
Team               Unit::getTeam() const { return team; }
char               Unit::getSymbol() const { return symbol; }
int                Unit::getHp() const { return hp; }
int                Unit::getMaxHp() const { return maxHp; }
int                Unit::getMoveRange() const { return moveRange; }
int                Unit::getAttackRange() const { return attackRange; }
int                Unit::getAttackPower() const { return attackPower; }
int                Unit::getDefensePower() const { return defensePower; }
int                Unit::getSpeed() const { return speed; }
bool               Unit::isAlive() const { return hp > 0; }

// ダメージ処理
void Unit::takeDamage(int dmg) {
    hp -= dmg;
    if (hp < 0) hp = 0;
}

void Unit::addWeapon(const WeaponData& weapon) {
    weapons_.push_back(weapon);
}

const WeaponData* Unit::getWeapon(int index) const {
    if (index >= 0 && index < static_cast<int>(weapons_.size())) {
        return &weapons_[index];
    }
    return nullptr;
}

void Unit::consumeAmmo(int weaponIndex) {
    if (weaponIndex >= 0 && weaponIndex < static_cast<int>(weapons_.size())) {
        if (weapons_[weaponIndex].ammo > 0) {
            weapons_[weaponIndex].ammo--;
        }
    }
}

void Unit::consumeEN(int amount) {
    currentEN_ -= amount;
    if (currentEN_ < 0) currentEN_ = 0;
}

void Unit::setEN(int current, int max) {
    currentEN_ = current;
    maxEN_ = max;
}

void Unit::recoverEN(int amount) {
    currentEN_ += amount;
    if (currentEN_ > maxEN_) {
        currentEN_ = maxEN_;
    }
}

// パイロット能力値取得（新規追加）
int Unit::getPilotFighting() const {
    return pilotData_ ? pilotData_->fighting : 100;
}

int Unit::getPilotShooting() const {
    return pilotData_ ? pilotData_->shooting : 100;
}

int Unit::getPilotDefense() const {
    return pilotData_ ? pilotData_->defense : 100;
}

int Unit::getPilotSkill() const {
    return pilotData_ ? pilotData_->skill : 100;
}

int Unit::getPilotReaction() const {
    return pilotData_ ? pilotData_->reaction : 100;
}

// 機体性能取得（新規追加）
int Unit::getArmor() const {
    return robotData_ ? robotData_->armor : defensePower;
}

int Unit::getMobility() const {
    return robotData_ ? robotData_->mobility : speed;
}

char Unit::getSize() const {
    return robotData_ ? robotData_->size : 'M';
}

char Unit::getTerrainAdaptation(char terrain) const {
    if (!robotData_) return 'B';

    switch (terrain) {
    case 'L': return robotData_->terrainLand;
    case 'S': return robotData_->terrainSea;
    case 'A': return robotData_->terrainAir;
    case 'W': return robotData_->terrainSpace;
    default:  return 'B';
    }
}