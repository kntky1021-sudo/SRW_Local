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

enum class Phase {
    Script,
    PlayerMove,
    PlayerAttack,
    EnemyMove,
    EnemyAttack
};

class ExecutionEngine {
public:
    ExecutionEngine(
        UIManager* uiMgr,
        InputManager* inputMgr,
        TileMap* tileMap_,
        Cursor* cursor_,
        BattleManager* battleMgr,
        int            windowW,
        int            windowH);

    void run(const std::string& scriptPath);
    void setHighlightTiles(const std::vector<std::pair<int, int>>& tiles);
    void redraw();

    // --- NEW PUBLIC GETTERS ---
    TileMap* getTileMap()       const { return tileMap; }
    Cursor* getCursor()        const { return cursor; }
    BattleManager* getBattleManager() const { return battleManager; }
    InputManager* getInput()         const { return input; }

private:
    UIManager* ui;
    InputManager* input;
    TileMap* tileMap;
    Cursor* cursor;
    BattleManager* battleManager;

    std::vector<std::unique_ptr<Command>> commands_;
    std::vector<Phase>                    commandPhases_;
    size_t                                currentIndex_;
    Camera                                camera_;
    Phase                                 currentPhase_;
    std::vector<std::pair<int, int>>       highlightTiles_;
};