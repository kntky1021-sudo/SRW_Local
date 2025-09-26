#pragma once

#include <string>
#include <vector>
#include <memory>
#include <utility>

#include "UIManager.h"
#include "InputManager.h"
#include "TileMap.h"
#include "Cursor.h"
#include "BattleManager.h"
#include "Camera.h"
#include "Command.h"

class ExecutionEngine {
public:
    ExecutionEngine(UIManager* uiMgr,
        InputManager* inputMgr,
        TileMap* tileMap,
        Cursor* cursor,
        BattleManager* battleMgr,
        int windowW,
        int windowH);

    /// スクリプト実行。省略時は "script.json"
    void run(const std::string& scriptPath = "script.json");

    /// 現在の状態を再描画
    void redraw();

    /// 移動可能範囲ハイライト用タイル座標セット
    void setHighlightTiles(const std::vector<std::pair<int, int>>& tiles);

    // コマンド実装から直接アクセス可能に
    UIManager* ui;
    InputManager* input;
    TileMap* tileMap;
    Cursor* cursor;
    BattleManager* battleManager;

    // ヘルパー
    Cursor* getCursor() const { return cursor; }
    BattleManager* getBattleManager() const { return battleManager; }
    void waitKey() { if (input) input->waitKey(); }
    int  waitForChoice(int maxChoices) { return input ? input->waitForChoice(maxChoices) : 0; }

private:
    std::vector<std::unique_ptr<Command>> commands_;
    std::size_t    currentIndex_;
    Camera         camera_;

    // 移動可能範囲ハイライトタイル
    std::vector<std::pair<int, int>> highlightTiles_;
};