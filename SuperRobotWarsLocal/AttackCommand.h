#pragma once

#include "Command.h"
#include <nlohmann/json.hpp>

/// @brief スクリプトから攻撃範囲を受け取り、敵ユニットを攻撃するコマンド
class AttackCommand : public Command {
public:
    explicit AttackCommand(const nlohmann::json& evt);
    void execute(ExecutionEngine& engine) override;

private:
    int unitId_;
    int attackRange_;
};