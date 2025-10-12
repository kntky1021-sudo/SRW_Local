#pragma once

#include <string>
#include <memory>
#include <vector>
#include <utility>
#include "BattleManager.h"
#include "TileMap.h"
#include "Cursor.h"
#include "UIManager.h"
#include "InputManager.h"
#include "SDLRenderer.h"
#include "Camera.h"
#include "BattleCalculator.h"
#include "UnitDatabase.h"  // ← 追加
#include <sstream>
#include "AudioManager.h" 

/// ゲーム全体の状態
enum class GameState {
    MainMenu,
    StageSelect,
    InBattle,
    PlayerTurn,
    EnemyTurn,
    BattleResult,
    GameOver,
    Exit
};

/// ゲーム全体を管理するクラス
class GameManager {
public:
    GameManager(
        UIManager* ui,
        InputManager* input,
        SDLRenderer* renderer,
        int windowW,
        int windowH
    );
    ~GameManager();

    void run();
    GameState getState() const { return state_; }

    // AudioManager設定（追加）
    void setAudioManager(AudioManager* audioMgr) { audioManager_ = audioMgr; }

private:
    // コンポーネント
    UIManager* ui_;
    InputManager* input_;
    SDLRenderer* renderer_;

    std::unique_ptr<TileMap> tileMap_;
    std::unique_ptr<Cursor> cursor_;
    std::unique_ptr<BattleManager> battleManager_;
    std::unique_ptr<Camera> camera_;

    UnitDatabase unitDB_;  // ← 追加

    // ゲーム状態
    GameState state_;
    int windowW_;
    int windowH_;
    int currentTurn_;

    // ユニット選択状態
    std::string selectedUnitId_;  // ← int から std::string に変更
    bool isSelectingDestination_;
    bool isShowingMenu_;
    bool isSelectingAttackTarget_;
    int menuCursor_;
    std::vector<std::pair<int, int>> movableArea_;
    std::vector<std::pair<int, int>> attackableArea_;

    // 状態別の処理
    void updateMainMenu();
    void updateStageSelect();
    void updatePlayerTurn();
    void updateEnemyTurn();
    void updateBattleResult();

    // 描画
    void render();
    void renderMainMenu();
    void renderBattle();
    void renderUI();
    void renderUnitInfo();

    // バトル開始
    void startBattle(const std::string& mapPath);

    // ユニット操作
    void selectUnit();
    void cancelSelection();
    void confirmMove();
    void showUnitMenu();
    void handleMenuInput();
    void selectAttack();
    void selectWait();
    void handleAttackTargetSelection();
    void executeAttack(int targetId);  // この引数は実際には使わない

    // ターン処理
    void startPlayerTurnPhase();
    void startEnemyTurnPhase();
    void endTurn();

    // 勝敗判定
    bool checkVictoryCondition();
    bool checkDefeatCondition();
    void playBattleAnimation(int attackerId, int defenderId);
    void showBattleResultDialog(const BattleResult& result);

    AudioManager* audioManager_;
};