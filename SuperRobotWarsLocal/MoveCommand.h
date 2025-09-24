#pragma once
#include <nlohmann/json.hpp>
#include "Command.h"
#include <array>

class ExecutionEngine;

class MoveCommand : public Command {
public:
    explicit MoveCommand(const nlohmann::json& evt);
    void execute(ExecutionEngine& engine) override;

private:
    int unitId_;
    std::array<int, 2> toPos_;
};