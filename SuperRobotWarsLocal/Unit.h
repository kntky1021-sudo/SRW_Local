#pragma once
#include <string>

enum class Team {
    Ally,
    Enemy
};

class Unit {
public:
    // コンストラクタ
    Unit(std::string id,
        Team team,
        char symbol,
        int maxHp,
        int moveRange,
        int attackRange,
        int attackPower,
        int defensePower,
        int speed);

    // 座標操作
    int  getX() const;
    int  getY() const;
    void setPosition(int x, int y);

    // ステータス取得
    const std::string& getId() const;
    Team               getTeam() const;
    char               getSymbol() const;
    int                getHp() const;
    int                getMaxHp() const;
    int                getMoveRange() const;
    int                getAttackRange() const;
    int                getAttackPower() const;
    int                getDefensePower() const;
    int                getSpeed() const;
    bool               isAlive() const;

    // ダメージ処理
    void takeDamage(int dmg);

private:
    std::string id;
    Team        team;
    char        symbol;
    int         hp;
    int         maxHp;
    int         moveRange;
    int         attackRange;
    int         attackPower;
    int         defensePower;
    int         speed;
    int         x = 0;
    int         y = 0;
};