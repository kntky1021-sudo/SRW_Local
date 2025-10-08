#include "Scenario.h"
#include "BattleManager.h"
#include <fstream>
#include <iostream>

bool Scenario::loadFromFile(const std::string& filepath) {
    std::ifstream ifs(filepath);
    if (!ifs) {
        std::cerr << "[Scenario] Cannot open file: " << filepath << "\n";
        return false;
    }

    nlohmann::json j;
    try {
        ifs >> j;
    }
    catch (const nlohmann::json::exception& e) {
        std::cerr << "[Scenario] JSON parse error: " << e.what() << "\n";
        return false;
    }

    // 基本情報
    data_.id = j.value("id", "");
    data_.title = j.value("title", "");
    data_.mapFile = j.value("mapFile", "");
    data_.maxTurns = j.value("maxTurns", 99);

    // 勝利条件
    std::string vicType = j.value("victoryType", "DestroyAllEnemies");
    if (vicType == "DestroyAllEnemies") data_.victoryType = VictoryCondition::DestroyAllEnemies;
    else if (vicType == "DestroyBoss") data_.victoryType = VictoryCondition::DestroyBoss;
    else if (vicType == "SurviveTurns") data_.victoryType = VictoryCondition::SurviveTurns;
    else if (vicType == "ReachPoint") data_.victoryType = VictoryCondition::ReachPoint;
    else data_.victoryType = VictoryCondition::Custom;

    data_.victoryTargetId = j.value("victoryTargetId", "");
    data_.victoryValue = j.value("victoryValue", 0);

    // 敗北条件
    std::string defType = j.value("defeatType", "AllAlliesDestroyed");
    if (defType == "AllAlliesDestroyed") data_.defeatType = DefeatCondition::AllAlliesDestroyed;
    else if (defType == "MainUnitDestroyed") data_.defeatType = DefeatCondition::MainUnitDestroyed;
    else if (defType == "TurnLimit") data_.defeatType = DefeatCondition::TurnLimit;
    else if (defType == "UnitDestroyed") data_.defeatType = DefeatCondition::UnitDestroyed;
    else data_.defeatType = DefeatCondition::Custom;

    data_.defeatTargetId = j.value("defeatTargetId", "");
    data_.defeatValue = j.value("defeatValue", 0);

    // 初期配置
    if (j.contains("initialUnits") && j["initialUnits"].is_array()) {
        for (const auto& unitJson : j["initialUnits"]) {
            ScenarioData::UnitPlacement placement;
            placement.unitId = unitJson.value("unitId", "");
            placement.robotId = unitJson.value("robotId", "");
            placement.pilotId = unitJson.value("pilotId", "");
            placement.x = unitJson.value("x", 0);
            placement.y = unitJson.value("y", 0);
            placement.isEnemy = unitJson.value("isEnemy", false);
            data_.initialUnits.push_back(placement);
        }
    }

    // イベント
    if (j.contains("events") && j["events"].is_array()) {
        for (const auto& evtJson : j["events"]) {
            data_.events.push_back(parseEvent(evtJson));
        }
    }

    // 会話
    data_.preBattleDialog = j.value("preBattleDialog", "");
    data_.postBattleDialog = j.value("postBattleDialog", "");
    data_.nextScenario = j.value("nextScenario", "");

    std::cout << "[Scenario] Loaded: " << data_.title << "\n";
    return true;
}

bool Scenario::checkVictoryCondition(BattleManager* bm, int currentTurn) {
    if (!bm) return false;

    switch (data_.victoryType) {
    case VictoryCondition::DestroyAllEnemies: {
        // TODO: 敵ユニットが全滅したかチェック
        // 現在の旧システムでは簡易実装
        std::cout << "[Scenario] Victory check: DestroyAllEnemies (stub)\n";
        return false;
    }

    case VictoryCondition::DestroyBoss: {
        // TODO: ボスが撃破されたかチェック
        std::cout << "[Scenario] Victory check: DestroyBoss (stub)\n";
        return false;
    }

    case VictoryCondition::SurviveTurns: {
        // 指定ターン数生存したか
        return currentTurn >= data_.victoryValue;
    }

    case VictoryCondition::ReachPoint: {
        // TODO: 指定地点到達チェック
        std::cout << "[Scenario] Victory check: ReachPoint (stub)\n";
        return false;
    }

    case VictoryCondition::ProtectUnit: {
        // TODO: 指定ユニット生存チェック
        std::cout << "[Scenario] Victory check: ProtectUnit (stub)\n";
        return false;
    }

    case VictoryCondition::Custom:
    default:
        return false;
    }
}

bool Scenario::checkDefeatCondition(BattleManager* bm, int currentTurn) {
    if (!bm) return false;

    switch (data_.defeatType) {
    case DefeatCondition::AllAlliesDestroyed: {
        // TODO: 味方全滅チェック
        std::cout << "[Scenario] Defeat check: AllAlliesDestroyed (stub)\n";
        return false;
    }

    case DefeatCondition::MainUnitDestroyed: {
        // TODO: 主人公機撃破チェック
        std::cout << "[Scenario] Defeat check: MainUnitDestroyed (stub)\n";
        return false;
    }

    case DefeatCondition::TurnLimit: {
        // ターン制限超過
        return currentTurn > data_.defeatValue;
    }

    case DefeatCondition::UnitDestroyed: {
        // TODO: 特定ユニット撃破チェック
        std::cout << "[Scenario] Defeat check: UnitDestroyed (stub)\n";
        return false;
    }

    case DefeatCondition::Custom:
    default:
        return false;
    }
}

void Scenario::checkEvents(BattleManager* bm, EventTrigger trigger, int currentTurn) {
    for (auto& event : data_.events) {
        if (event.executed) continue;
        if (event.trigger != trigger) continue;

        bool shouldExecute = false;

        switch (trigger) {
        case EventTrigger::TurnStart:
        case EventTrigger::TurnEnd:
            shouldExecute = (event.turnNumber == currentTurn);
            break;

        case EventTrigger::Custom:
            if (event.customCondition) {
                shouldExecute = event.customCondition(bm);
            }
            break;

        default:
            break;
        }

        if (shouldExecute) {
            // イベント実行
            std::cout << "[Scenario] Event triggered\n";

            // 会話表示
            if (!event.dialogFile.empty()) {
                std::cout << "[Scenario] Show dialog: " << event.dialogFile << "\n";
            }

            // 増援登場
            for (const auto& reinforcementId : event.reinforcements) {
                std::cout << "[Scenario] Reinforcement: " << reinforcementId << "\n";
            }

            // カスタムアクション
            if (event.customAction) {
                event.customAction(bm);
            }

            event.executed = true;
        }
    }
}

void Scenario::triggerEvent(BattleManager* bm, EventTrigger trigger, const std::string& unitId) {
    for (auto& event : data_.events) {
        if (event.executed) continue;
        if (event.trigger != trigger) continue;

        bool shouldExecute = false;

        switch (trigger) {
        case EventTrigger::UnitDestroyed:
            shouldExecute = (event.unitId == unitId);
            break;

        case EventTrigger::ReachPosition:
            // TODO: 座標チェック
            break;

        default:
            break;
        }

        if (shouldExecute) {
            std::cout << "[Scenario] Event triggered for unit: " << unitId << "\n";

            if (!event.dialogFile.empty()) {
                std::cout << "[Scenario] Show dialog: " << event.dialogFile << "\n";
            }

            if (event.customAction) {
                event.customAction(bm);
            }

            event.executed = true;
        }
    }
}

ScenarioEvent Scenario::parseEvent(const nlohmann::json& j) {
    ScenarioEvent event;

    std::string triggerStr = j.value("trigger", "TurnStart");
    if (triggerStr == "TurnStart") event.trigger = EventTrigger::TurnStart;
    else if (triggerStr == "TurnEnd") event.trigger = EventTrigger::TurnEnd;
    else if (triggerStr == "UnitDestroyed") event.trigger = EventTrigger::UnitDestroyed;
    else if (triggerStr == "ReachPosition") event.trigger = EventTrigger::ReachPosition;
    else if (triggerStr == "HpBelow") event.trigger = EventTrigger::HpBelow;
    else event.trigger = EventTrigger::Custom;

    event.turnNumber = j.value("turnNumber", 0);
    event.unitId = j.value("unitId", "");
    event.x = j.value("x", 0);
    event.y = j.value("y", 0);
    event.value = j.value("value", 0);
    event.dialogFile = j.value("dialogFile", "");

    if (j.contains("reinforcements") && j["reinforcements"].is_array()) {
        for (const auto& r : j["reinforcements"]) {
            event.reinforcements.push_back(r.get<std::string>());
        }
    }

    return event;
}