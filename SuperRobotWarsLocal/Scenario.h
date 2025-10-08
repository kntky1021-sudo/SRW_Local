#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>

class BattleManager;
class Unit;

/// 勝利条件の種類
enum class VictoryCondition {
    DestroyAllEnemies,    // 敵全滅
    DestroyBoss,          // ボス撃破
    SurviveTurns,         // ○ターン生存
    ReachPoint,           // 指定地点到達
    ProtectUnit,          // ユニット防衛
    Custom                // カスタム条件
};

/// 敗北条件の種類
enum class DefeatCondition {
    AllAlliesDestroyed,   // 味方全滅
    MainUnitDestroyed,    // 主人公機撃破
    TurnLimit,            // ターン制限超過
    UnitDestroyed,        // 特定ユニット撃破
    Custom                // カスタム条件
};

/// イベントトリガーの種類
enum class EventTrigger {
    TurnStart,            // ターン開始時
    TurnEnd,              // ターン終了時
    UnitDestroyed,        // ユニット撃破時
    ReachPosition,        // 指定座標到達
    HpBelow,              // HP指定値以下
    Custom                // カスタム条件
};

/// イベントデータ
struct ScenarioEvent {
    EventTrigger trigger;
    int turnNumber = 0;           // ターン指定（TurnStart/TurnEnd用）
    std::string unitId;           // ユニット指定
    int x = 0, y = 0;             // 座標指定
    int value = 0;                // 汎用値（HP閾値など）
    std::string dialogFile;       // 会話ファイル
    std::vector<std::string> reinforcements;  // 増援ユニットID
    bool executed = false;        // 実行済みフラグ

    // カスタム条件関数
    std::function<bool(BattleManager*)> customCondition;
    // カスタム実行関数
    std::function<void(BattleManager*)> customAction;
};

/// シナリオデータ
struct ScenarioData {
    std::string id;
    std::string title;
    std::string mapFile;
    int maxTurns = 99;

    // 勝利・敗北条件
    VictoryCondition victoryType;
    std::string victoryTargetId;  // ボスユニットIDなど
    int victoryValue = 0;         // ターン数、座標など

    DefeatCondition defeatType;
    std::string defeatTargetId;
    int defeatValue = 0;

    // 初期配置
    struct UnitPlacement {
        std::string unitId;
        std::string robotId;
        std::string pilotId;
        int x, y;
        bool isEnemy;
    };
    std::vector<UnitPlacement> initialUnits;

    // イベント
    std::vector<ScenarioEvent> events;

    // 会話
    std::string preBattleDialog;   // 戦闘前会話
    std::string postBattleDialog;  // 戦闘後会話

    // 次のシナリオ
    std::string nextScenario;
};

/// シナリオ管理クラス
class Scenario {
public:
    Scenario() = default;

    /// JSONファイルからシナリオを読み込み
    bool loadFromFile(const std::string& filepath);

    /// シナリオデータを取得
    const ScenarioData& getData() const { return data_; }
    ScenarioData& getData() { return data_; }

    /// 勝利条件チェック
    bool checkVictoryCondition(BattleManager* bm, int currentTurn);

    /// 敗北条件チェック
    bool checkDefeatCondition(BattleManager* bm, int currentTurn);

    /// イベントチェックと実行
    void checkEvents(BattleManager* bm, EventTrigger trigger, int currentTurn);

    /// 特定イベントのトリガー（ユニット撃破時など）
    void triggerEvent(BattleManager* bm, EventTrigger trigger,
        const std::string& unitId = "");

private:
    ScenarioData data_;

    /// JSON解析
    ScenarioEvent parseEvent(const nlohmann::json& j);
};