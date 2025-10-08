#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

/// ユニットの永続データ
struct UnitSaveData {
    std::string unitId;
    std::string robotId;
    std::string pilotId;
    int level;
    int experience;
    int kills;  // 撃墜数

    // 改造レベル（後で実装）
    int hpUpgrade = 0;
    int enUpgrade = 0;
    int armorUpgrade = 0;
    int mobilityUpgrade = 0;
    int weaponUpgrade = 0;

    // 装備中の強化パーツ（後で実装）
    std::vector<std::string> parts;
};

/// プレイヤーの進行状況
struct ProgressData {
    std::string currentScenario;    // 現在のシナリオ
    int money;                      // 資金
    int totalTurns;                 // 累計ターン数
    int totalKills;                 // 累計撃墜数

    // クリア済みシナリオ
    std::vector<std::string> clearedScenarios;

    // 隠し要素フラグ
    std::unordered_map<std::string, bool> flags;

    // 獲得した強化パーツ
    std::vector<std::string> ownedParts;
};

/// セーブデータ管理
class SaveData {
public:
    SaveData() = default;

    /// セーブデータを読み込み
    bool load(const std::string& filepath);

    /// セーブデータを保存
    bool save(const std::string& filepath) const;

    /// ユニットデータ登録・更新
    void registerUnit(const UnitSaveData& unitData);
    void updateUnitFromBattle(const class Unit* unit);

    /// ユニットデータ取得
    const UnitSaveData* getUnitData(const std::string& unitId) const;
    UnitSaveData* getUnitData(const std::string& unitId);

    /// 進行状況取得
    ProgressData& getProgress() { return progress_; }
    const ProgressData& getProgress() const { return progress_; }

    /// 資金操作
    void addMoney(int amount) { progress_.money += amount; }
    void spendMoney(int amount) { progress_.money -= amount; }
    int getMoney() const { return progress_.money; }

    /// シナリオクリア登録
    void markScenarioCleared(const std::string& scenarioId);
    bool isScenarioCleared(const std::string& scenarioId) const;

    /// フラグ操作
    void setFlag(const std::string& flagName, bool value);
    bool getFlag(const std::string& flagName) const;

    /// 強化パーツ取得
    void addPart(const std::string& partId);
    bool hasPart(const std::string& partId) const;

    /// 全ユニットデータ取得
    const std::vector<UnitSaveData>& getAllUnits() const { return units_; }

private:
    std::vector<UnitSaveData> units_;
    ProgressData progress_;

    // JSON変換
    nlohmann::json unitToJson(const UnitSaveData& unit) const;
    UnitSaveData jsonToUnit(const nlohmann::json& j) const;
};