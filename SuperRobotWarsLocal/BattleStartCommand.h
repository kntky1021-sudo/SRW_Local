#pragma once

#include "Command.h"
#include <nlohmann/json.hpp>
#include <unordered_map>

class BattleStartCommand : public Command {
public:
    explicit BattleStartCommand(const nlohmann::json& evt);
    void execute(ExecutionEngine& engine) override;

private:
    int                                  mainUnitId_;
    std::vector<int>                    enemyIds_;
    std::string                          mapPath_;
    std::unordered_map<int, std::pair<int, int>> initPositions_;
};