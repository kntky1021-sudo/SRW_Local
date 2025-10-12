#include "BattleCalculator.h"
#include "Unit.h"
#include "UnitData.h"
#include <random>
#include <algorithm>
#include <iostream>

namespace {
    // 乱数生成器
    std::random_device rd;
    std::mt19937 gen(rd());
}

int BattleCalculator::random100() {
    std::uniform_int_distribution<> dis(0, 99);
    return dis(gen);
}

float BattleCalculator::getTerrainModifier(char adaptation) {
    switch (adaptation) {
    case 'S': return 1.2f;  // 最適地形
    case 'A': return 1.1f;  // 良好
    case 'B': return 1.0f;  // 普通
    case 'C': return 0.9f;  // 不利
    case 'D': return 0.7f;  // 大幅不利
    default:  return 1.0f;
    }
}

BattleResult BattleCalculator::execute(
    Unit* attacker,
    Unit* defender,
    const WeaponData* weapon,
    char terrain)
{
    BattleResult result{};

    if (!attacker || !defender) {
        std::cerr << "[BattleCalculator] Invalid parameters\n";
        return result;
    }

    std::cout << "[BattleCalculator] Battle: " << attacker->getId()
        << " vs " << defender->getId() << "\n";

    // 1) 命中判定
    int hitRate = calculateHitRate(attacker, defender, weapon, terrain);
    int hitRoll = random100();

    std::cout << "  Hit rate: " << hitRate << "%, Roll: " << hitRoll << "\n";

    if (hitRoll >= hitRate) {
        // 攻撃失敗
        std::cout << "  Attack MISSED!\n";
        result.damage = 0;
        result.isCritical = false;
        return result;
    }

    // 2) クリティカル判定
    int critRate = calculateCriticalRate(attacker, weapon);
    int critRoll = random100();
    result.isCritical = (critRoll < critRate);

    std::cout << "  Critical rate: " << critRate << "%, Roll: " << critRoll
        << (result.isCritical ? " -> CRITICAL!" : "") << "\n";

    // 3) ダメージ計算
    result.damage = calculateDamage(attacker, defender, weapon, terrain, result.isCritical);

    std::cout << "  Damage: " << result.damage << "\n";

    // 4) 反撃判定（射程内かつ反撃可能武器がある場合）
    // TODO: 実際の反撃システム実装
    result.isCountered = false;
    result.counterDamage = 0;

    // 5) 撃破判定
    int defenderHpAfter = defender->getHp() - result.damage;
    result.defenderDestroyed = (defenderHpAfter <= 0);
    result.attackerDestroyed = false;

    // 6) 経験値計算
    bool damaged = (result.damage > 0);
    result.expGained = calculateExperience(attacker, defender, damaged, result.defenderDestroyed);

    std::cout << "  Exp gained: " << result.expGained << "\n";

    return result;
}

int BattleCalculator::calculateHitRate(
    const Unit* attacker,
    const Unit* defender,
    const WeaponData* weapon,
    char terrain)
{
    // パイロット技量と機体運動性を使用
    int pilotSkill = attacker->getPilotSkill();
    int robotMobility = attacker->getMobility();
    int weaponAccuracy = 80;  // 武器基本命中率（後で武器データから）

    int baseHit = weaponAccuracy + (pilotSkill / 2) + (robotMobility / 10);

    // 地形適応による補正
    char adaptation = attacker->getTerrainAdaptation(terrain);
    float terrainMod = getTerrainModifier(adaptation);
    baseHit = static_cast<int>(baseHit * terrainMod);

    // 回避率を引く
    int evadeRate = calculateEvadeRate(defender, terrain);

    int finalHit = baseHit - evadeRate;

    return std::clamp(finalHit, 0, 100);
}

int BattleCalculator::calculateEvadeRate(
    const Unit* defender,
    char terrain)
{
    // パイロット反応と機体運動性を使用
    int pilotReaction = defender->getPilotReaction();
    int robotMobility = defender->getMobility();

    int evadeRate = (pilotReaction + robotMobility) / 2;

    // 地形適応補正
    char adaptation = defender->getTerrainAdaptation(terrain);
    float terrainMod = getTerrainModifier(adaptation);
    evadeRate = static_cast<int>(evadeRate * terrainMod);

    return std::clamp(evadeRate, 0, 100);
}

int BattleCalculator::calculateCriticalRate(
    const Unit* attacker,
    const WeaponData* weapon)
{
    // パイロット技量を使用
    int pilotSkill = attacker->getPilotSkill();
    int weaponCritBonus = 10;  // 武器のクリティカル補正

    int critRate = (pilotSkill / 10) + weaponCritBonus;

    return std::clamp(critRate, 0, 50);
}

int BattleCalculator::calculateDamage(
    const Unit* attacker,
    const Unit* defender,
    const WeaponData* weapon,
    char terrain,
    bool isCritical)
{
    // 武器威力
    int weaponPower = weapon ? weapon->power : attacker->getAttackPower();

    // 武器属性に応じてパイロット能力を選択
    // TODO: 武器の属性（格闘/射撃）で分岐
    int pilotAttack = attacker->getPilotShooting();  // 仮で射撃を使用

    // 防御側の装甲
    int defenderArmor = defender->getArmor();

    // 基本ダメージ = (武器威力 + パイロット能力) - (装甲 / 10)
    int baseDamage = weaponPower + pilotAttack - (defenderArmor / 10);

    // 地形適応補正
    char attackerAdaptation = attacker->getTerrainAdaptation(terrain);
    float terrainMod = getTerrainModifier(attackerAdaptation);
    baseDamage = static_cast<int>(baseDamage * terrainMod);

    // サイズ補正（大型ほど被ダメージ増加）
    char defenderSize = defender->getSize();
    float sizeMod = 1.0f;
    switch (defenderSize) {
    case 'S': sizeMod = 0.8f; break;
    case 'M': sizeMod = 1.0f; break;
    case 'L': sizeMod = 1.2f; break;
    case 'X': sizeMod = 1.5f; break;  // LL（超大型）
    }
    baseDamage = static_cast<int>(baseDamage * sizeMod);

    // クリティカル時は1.5倍
    if (isCritical) {
        baseDamage = static_cast<int>(baseDamage * 1.5f);
    }

    // 最低ダメージは10%保証
    int minDamage = weaponPower / 10;
    baseDamage = std::max(baseDamage, minDamage);

    // 乱数補正 ±10%
    int variance = (random100() - 50) / 5;
    int finalDamage = baseDamage + (baseDamage * variance / 100);

    return std::max(finalDamage, 1);
}

int BattleCalculator::calculateExperience(
    const Unit* attacker,
    const Unit* defender,
    bool damaged,
    bool destroyed)
{
    // SFC版スパロボ風経験値計算

    int baseExp = 10;  // 基本経験値

    if (!damaged) {
        // 攻撃が当たらなかった場合
        return baseExp / 2;  // 5
    }

    // レベル差補正（後で実装）
    // int levelDiff = defender->getLevel() - attacker->getLevel();
    // baseExp += levelDiff * 5;

    // 撃破ボーナス
    if (destroyed) {
        baseExp += 50;
    }

    return std::max(baseExp, 5);  // 最低5経験値
}