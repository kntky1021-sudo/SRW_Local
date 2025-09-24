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