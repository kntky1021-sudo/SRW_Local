#pragma once

#include "Command.h"
#include <nlohmann/json.hpp>

class AttackCommand : public Command {
public:
    explicit AttackCommand(const nlohmann::json& evt);
    void execute(ExecutionEngine& engine) override;

private:
    int targetX_;
    int targetY_;
};