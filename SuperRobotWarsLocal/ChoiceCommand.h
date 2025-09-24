#pragma once

#include "Command.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

class ChoiceCommand : public Command {
public:
    explicit ChoiceCommand(const nlohmann::json& evt);
    void execute(ExecutionEngine& engine) override;

private:
    std::string              prompt_;
    std::vector<std::string> options_;
    std::vector<std::string> resultLabels_;
};