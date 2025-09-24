#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include "Command.h"

class ExecutionEngine;

class BattleStartCommand : public Command {
public:
    explicit BattleStartCommand(const nlohmann::json& evt);
    void execute(ExecutionEngine& engine) override;

private:
    std::string mapSpec_; // JSONから受け取った生の指定（例: "maps/map01.json" など）

    static std::string normalizeMapPath(const std::string& spec);
};