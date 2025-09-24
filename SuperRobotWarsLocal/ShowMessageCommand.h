#pragma once

#include "Command.h"
#include <nlohmann/json.hpp>
#include <string>

class ShowMessageCommand : public Command {
public:
    explicit ShowMessageCommand(const nlohmann::json& evt);
    void execute(ExecutionEngine& engine) override;

private:
    std::string text_;
};