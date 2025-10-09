#include "UnitDatabase.h"
#include <fstream>
#include <iostream>

bool UnitDatabase::loadRobots(const std::string& filepath) {
    std::ifstream ifs(filepath);
    if (!ifs) {
        std::cerr << "[UnitDatabase] Cannot open robots file: " << filepath << "\n";
        return false;
    }

    nlohmann::json j;
    try {
        ifs >> j;
    }
    catch (const nlohmann::json::exception& e) {
        std::cerr << "[UnitDatabase] JSON parse error: " << e.what() << "\n";
        return false;
    }

    if (!j.is_array()) {
        std::cerr << "[UnitDatabase] Robots JSON is not an array\n";
        return false;
    }

    for (const auto& robotJson : j) {
        RobotData robot = parseRobot(robotJson);
        robots_[robot.id] = robot;
        std::cout << "[UnitDatabase] Loaded robot: " << robot.name << " (" << robot.id << ")\n";
    }

    std::cout << "[UnitDatabase] Total robots loaded: " << robots_.size() << "\n";
    return true;
}

bool UnitDatabase::loadPilots(const std::string& filepath) {
    std::ifstream ifs(filepath);
    if (!ifs) {
        std::cerr << "[UnitDatabase] Cannot open pilots file: " << filepath << "\n";
        return false;
    }

    nlohmann::json j;
    try {
        ifs >> j;
    }
    catch (const nlohmann::json::exception& e) {
        std::cerr << "[UnitDatabase] JSON parse error: " << e.what() << "\n";
        return false;
    }

    if (!j.is_array()) {
        std::cerr << "[UnitDatabase] Pilots JSON is not an array\n";
        return false;
    }

    for (const auto& pilotJson : j) {
        PilotData pilot = parsePilot(pilotJson);
        pilots_[pilot.id] = pilot;
        std::cout << "[UnitDatabase] Loaded pilot: " << pilot.name << " (" << pilot.id << ")\n";
    }

    std::cout << "[UnitDatabase] Total pilots loaded: " << pilots_.size() << "\n";
    return true;
}

const RobotData* UnitDatabase::getRobot(const std::string& robotId) const {
    auto it = robots_.find(robotId);
    if (it != robots_.end()) {
        return &it->second;
    }
    return nullptr;
}

const PilotData* UnitDatabase::getPilot(const std::string& pilotId) const {
    auto it = pilots_.find(pilotId);
    if (it != pilots_.end()) {
        return &it->second;
    }
    return nullptr;
}

std::unique_ptr<Unit> UnitDatabase::createUnit(
    const std::string& unitId,
    const std::string& robotId,
    const std::string& pilotId,
    Team team) const
{
    const RobotData* robot = getRobot(robotId);
    const PilotData* pilot = getPilot(pilotId);

    if (!robot) {
        std::cerr << "[UnitDatabase] Robot not found: " << robotId << "\n";
        return nullptr;
    }

    if (!pilot) {
        std::cerr << "[UnitDatabase] Pilot not found: " << pilotId << "\n";
        return nullptr;
    }

    // ユニット作成（機体データ+パイロットデータを統合）
    auto unit = std::make_unique<Unit>(
        unitId,
        team,
        (team == Team::Ally ? 'P' : 'E'),
        robot->maxHp,
        robot->moveRange,
        (!robot->weapons.empty() ? robot->weapons[0].maxRange : 1),
        (!robot->weapons.empty() ? robot->weapons[0].power : 100),
        robot->armor,
        robot->mobility
    );

    // パイロット/機体データを設定
    unit->setRobotData(robot);
    unit->setPilotData(pilot);

    // ENを設定
    unit->setEN(robot->maxEn, robot->maxEn);

    // 武器を追加
    for (const auto& weaponData : robot->weapons) {
        unit->addWeapon(weaponData);
    }

    std::cout << "[UnitDatabase] Created unit: " << robot->name
        << " piloted by " << pilot->name
        << " (Skill:" << pilot->skill
        << " Mobility:" << robot->mobility << ")\n";

    return unit;
}

std::vector<std::string> UnitDatabase::getRobotIds() const {
    std::vector<std::string> ids;
    ids.reserve(robots_.size());
    for (const auto& pair : robots_) {
        ids.push_back(pair.first);
    }
    return ids;
}

std::vector<std::string> UnitDatabase::getPilotIds() const {
    std::vector<std::string> ids;
    ids.reserve(pilots_.size());
    for (const auto& pair : pilots_) {
        ids.push_back(pair.first);
    }
    return ids;
}

WeaponData UnitDatabase::parseWeapon(const nlohmann::json& j) const {
    WeaponData weapon;
    weapon.name = j.value("name", "");
    weapon.power = j.value("power", 0);
    weapon.minRange = j.value("minRange", 1);
    weapon.maxRange = j.value("maxRange", 1);
    weapon.ammo = j.value("ammo", -1);
    weapon.enCost = j.value("enCost", 0);
    weapon.isMap = j.value("isMap", false);
    weapon.attribute = j.value("attribute", "");
    return weapon;
}

RobotData UnitDatabase::parseRobot(const nlohmann::json& j) const {
    RobotData robot;
    robot.id = j.value("id", "");
    robot.name = j.value("name", "");
    robot.series = j.value("series", "");
    robot.maxHp = j.value("maxHp", 1000);
    robot.maxEn = j.value("maxEn", 100);
    robot.armor = j.value("armor", 100);
    robot.mobility = j.value("mobility", 100);
    robot.moveRange = j.value("moveRange", 5);
    robot.movementType = j.value("movementType", "L")[0];
    robot.terrainLand = j.value("terrainLand", "B")[0];
    robot.terrainSea = j.value("terrainSea", "B")[0];
    robot.terrainAir = j.value("terrainAir", "B")[0];
    robot.terrainSpace = j.value("terrainSpace", "B")[0];
    robot.size = j.value("size", "M")[0];

    // 武器読み込み
    if (j.contains("weapons") && j["weapons"].is_array()) {
        for (const auto& weaponJson : j["weapons"]) {
            robot.weapons.push_back(parseWeapon(weaponJson));
        }
    }

    return robot;
}

PilotData UnitDatabase::parsePilot(const nlohmann::json& j) const {
    PilotData pilot;
    pilot.id = j.value("id", "");
    pilot.name = j.value("name", "");
    pilot.series = j.value("series", "");
    pilot.fighting = j.value("fighting", 100);
    pilot.shooting = j.value("shooting", 100);
    pilot.defense = j.value("defense", 100);
    pilot.skill = j.value("skill", 100);
    pilot.reaction = j.value("reaction", 100);

    // 精神コマンド読み込み
    if (j.contains("spirits") && j["spirits"].is_array()) {
        for (const auto& spirit : j["spirits"]) {
            if (spirit.is_string()) {
                pilot.spirits.push_back(spirit.get<std::string>());
            }
        }
    }

    // 特殊能力読み込み
    if (j.contains("abilities") && j["abilities"].is_array()) {
        for (const auto& ability : j["abilities"]) {
            if (ability.is_string()) {
                pilot.abilities.push_back(ability.get<std::string>());
            }
        }
    }

    return pilot;
}