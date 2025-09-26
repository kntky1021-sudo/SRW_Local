#pragma once

#include "Command.h"
#include <nlohmann/json.hpp>

/// @brief 簡易 AI による敵ユニット移動コマンド
class EnemyMoveCommand : public Command {
public:
    explicit EnemyMoveCommand(const nlohmann::json& evt);
    void execute(ExecutionEngine& engine) override;

private:
    int unitId_;
    int moveRange_;
};