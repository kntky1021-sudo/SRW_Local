#pragma once

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "SDLRenderer.h"
#include "Unit.h"

// 前方宣言
class UIManager;

/// BattleManager (完全改修版)
/// 完全なUnitクラスを使用してユニット管理
class BattleManager {
public:
    explicit BattleManager(SDLRenderer* renderer);
    ~BattleManager();

    // マップ開始用
    void startBattle(const std::string& mapName);

    // ユニット登録（新方式）
    void registerUnit(std::unique_ptr<Unit> unit, int x, int y);

    // ユニット取得
    Unit* getUnitById(const std::string& unitId);
    Unit* getUnitAt(int x, int y);
    const Unit* getUnitAt(int x, int y) const;

    // ユニット移動
    void moveUnit(const std::string& unitId, int x, int y);

    // 攻撃実行
    void attack(const std::string& attackerId, const std::string& defenderId);

    // ユニットレンダリング
    void renderUnits(SDLRenderer* renderer, int offsetX, int offsetY) const;

    // 行動済みフラグ管理
    void setUnitActed(const std::string& unitId, bool acted);
    bool hasUnitActed(const std::string& unitId) const;
    void resetAllActedFlags(bool enemyOnly = false);

    // ユニット情報取得
    bool isEnemyUnit(const std::string& unitId) const;
    std::string getUnitName(const std::string& unitId) const;
    int getUnitCount() const { return static_cast<int>(units_.size()); }

    // 敵味方の生存数取得
    int getAliveAllyCount() const;
    int getAliveEnemyCount() const;

    // 経験値・レベル管理
    void addExperience(const std::string& unitId, int exp);
    bool checkLevelUp(const std::string& unitId);

    // 全ユニットのIDリスト取得
    std::vector<std::string> getAllUnitIds() const;
    std::vector<std::string> getAllyUnitIds() const;
    std::vector<std::string> getEnemyUnitIds() const;

    // デバッグ用：全ユニット情報表示
    void printAllUnits() const;

private:
    SDLRenderer* renderer_;

    // ユニット管理（新方式）
    std::vector<std::unique_ptr<Unit>> units_;
    std::unordered_map<std::string, Unit*> unitMap_;  // ID→Unitの高速検索用

    // 行動済みフラグ（ユニットIDで管理）
    std::unordered_map<std::string, bool> actedFlags_;

    // 内部ヘルパー
    Unit* findUnitById(const std::string& unitId);
    const Unit* findUnitById(const std::string& unitId) const;
};