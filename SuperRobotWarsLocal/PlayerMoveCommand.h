#pragma once
#include <nlohmann/json.hpp>
#include "Command.h"

class ExecutionEngine;

class PlayerMoveCommand : public Command {
public:
    explicit PlayerMoveCommand(const nlohmann::json& evt);
    void execute(ExecutionEngine& engine) override;

private:
    int  unitId_;
    bool snapCursorToUnit_;  // 実行前にカーソルをユニット位置へ移動するか
};