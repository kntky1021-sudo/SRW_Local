// include/EnemyAttackCommand.h

#pragma once

#include "Command.h"
#include <nlohmann/json.hpp>

/// @brief 簡易 AI による敵ユニット攻撃コマンド
class EnemyAttackCommand : public Command {
public:
	explicit EnemyAttackCommand(const nlohmann::json& evt);
	void execute(ExecutionEngine& engine) override;

private:
	int unitId_;
	int attackRange_;
};