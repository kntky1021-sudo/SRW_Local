#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <nlohmann/json.hpp>
#include "UnitData.h"
#include "Unit.h"

/// ユニットデータベース
/// JSONファイルから機体・パイロット情報を読み込み管理
class UnitDatabase {
public:
    UnitDatabase() = default;

    // データベース読み込み
    bool loadRobots(const std::string& filepath);
    bool loadPilots(const std::string& filepath);

    // データ取得
    const RobotData* getRobot(const std::string& robotId) const;
    const PilotData* getPilot(const std::string& pilotId) const;

    // ユニット生成
    std::unique_ptr<Unit> createUnit(
        const std::string& unitId,
        const std::string& robotId,
        const std::string& pilotId,
        Team team) const;

    // デバッグ用：登録されている全機体・パイロットのリスト取得
    std::vector<std::string> getRobotIds() const;
    std::vector<std::string> getPilotIds() const;

private:
    std::unordered_map<std::string, RobotData> robots_;
    std::unordered_map<std::string, PilotData> pilots_;

    // JSON解析ヘルパー
    WeaponData parseWeapon(const nlohmann::json& j) const;
    RobotData parseRobot(const nlohmann::json& j) const;
    PilotData parsePilot(const nlohmann::json& j) const;
};