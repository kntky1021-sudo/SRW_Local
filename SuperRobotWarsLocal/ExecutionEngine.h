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
    enum class Phase {
        Script,
        PlayerMove,
        PlayerAttack,
        EnemyMove,
        EnemyAttack
    };

    ExecutionEngine(UIManager* uiMgr,
        InputManager* inputMgr,
        TileMap* tileMap,
        Cursor* cursor,
        BattleManager* battleMgr,
        int windowW,
        int windowH);

    void run(const std::string& scriptPath = "script.json");
    void redraw();
    void setHighlightTiles(const std::vector<std::pair<int, int>>& tiles);

    // 実行中のコマンドが参照できるように
    Phase getCurrentPhase() const { return currentPhase_; }
    Cursor* getCursor() const { return cursor; }
    BattleManager* getBattleManager() const { return battleManager; }
    void waitKey() { if (input) input->waitKey(); }

    UIManager* ui;
    InputManager* input;
    TileMap* tileMap;
    Cursor* cursor;
    BattleManager* battleManager;

private:
    std::vector<std::unique_ptr<Command>> commands_;
    // コマンドごとのフェイズを同じ順序で保持
    std::vector<Phase>                   commandPhases_;
    std::size_t    currentIndex_;
    Camera         camera_;
    std::vector<std::pair<int, int>> highlightTiles_;
    Phase          currentPhase_;
};