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
#include "BattleCalculator.h"  // これをファイル先頭のインクルード部分に追加
#include <sstream>  // これもファイル先頭に追加

/// ゲーム全体の状態
enum class GameState {
    MainMenu,           // メインメニュー
    StageSelect,        // ステージ選択
    InBattle,           // 戦闘中
    PlayerTurn,         // プレイヤーターン
    EnemyTurn,          // 敵ターン
    BattleResult,       // 戦闘結果
    GameOver,           // ゲームオーバー
    Exit                // 終了
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

    /// ゲームメインループ
    void run();

    /// 現在の状態取得
    GameState getState() const { return state_; }

private:
    // コンポーネント
    UIManager* ui_;
    InputManager* input_;
    SDLRenderer* renderer_;

    std::unique_ptr<TileMap> tileMap_;
    std::unique_ptr<Cursor> cursor_;
    std::unique_ptr<BattleManager> battleManager_;
    std::unique_ptr<Camera> camera_;

    // ゲーム状態
    GameState state_;
    int windowW_;
    int windowH_;
    int currentTurn_;

    // ユニット選択状態
    int selectedUnitId_;              // 選択中のユニットID（-1=未選択）
    bool isSelectingDestination_;     // 移動先選択中か
    bool isShowingMenu_;              // メニュー表示中か
    bool isSelectingAttackTarget_;    // 攻撃対象選択中か
    int menuCursor_;                  // メニューカーソル位置
    std::vector<std::pair<int, int>> movableArea_;   // 移動可能範囲
    std::vector<std::pair<int, int>> attackableArea_; // 攻撃可能範囲

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
    void renderUI();           // UI要素の描画
    void renderUnitInfo();     // ユニット情報表示

    // バトル開始
    void startBattle(const std::string& mapPath);

    // ユニット操作
    void selectUnit();           // カーソル位置のユニットを選択
    void cancelSelection();      // 選択をキャンセル
    void confirmMove();          // 移動を確定
    void showUnitMenu();         // ユニットメニュー表示
    void handleMenuInput();      // メニュー入力処理
    void selectAttack();         // 攻撃選択
    void selectWait();           // 待機選択
    void handleAttackTargetSelection(); // 攻撃対象選択処理
    void executeAttack(int targetId);   // 攻撃実行

    // ターン処理
    void startPlayerTurnPhase();
    void startEnemyTurnPhase();
    void endTurn();

    // 勝敗判定
    bool checkVictoryCondition();
    bool checkDefeatCondition();
    void playBattleAnimation(int attackerId, int defenderId);
    void showBattleResultDialog(const BattleResult& result);
};