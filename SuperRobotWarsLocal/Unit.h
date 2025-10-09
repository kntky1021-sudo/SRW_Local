#pragma once
#include <string>
#include <vector>
#include "UnitData.h"

enum class Team {
    Ally,
    Enemy
};

class Unit {
public:
    // コンストラクタ（既存）
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

    // 武器関連
    void addWeapon(const WeaponData& weapon);
    const std::vector<WeaponData>& getWeapons() const { return weapons_; }
    const WeaponData* getWeapon(int index) const;
    void consumeAmmo(int weaponIndex);
    void consumeEN(int amount);
    int getCurrentEN() const { return currentEN_; }
    int getMaxEN() const { return maxEN_; }
    void setEN(int current, int max);
    void recoverEN(int amount = 10);

    // パイロット/機体データ設定（新規）
    void setRobotData(const RobotData* robotData) { robotData_ = robotData; }
    void setPilotData(const PilotData* pilotData) { pilotData_ = pilotData; }

    // パイロット/機体データ取得
    const RobotData* getRobotData() const { return robotData_; }
    const PilotData* getPilotData() const { return pilotData_; }

    // パイロット能力値取得
    int getPilotFighting() const;
    int getPilotShooting() const;
    int getPilotDefense() const;
    int getPilotSkill() const;
    int getPilotReaction() const;

    // 機体性能取得
    int getArmor() const;
    int getMobility() const;
    char getSize() const;
    char getTerrainAdaptation(char terrain) const;

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

    // 武器管理
    std::vector<WeaponData> weapons_;
    int currentEN_;
    int maxEN_;

    // パイロット/機体データへの参照（新規）
    const RobotData* robotData_ = nullptr;
    const PilotData* pilotData_ = nullptr;
};