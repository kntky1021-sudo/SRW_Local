#pragma once
#include "Command.h"
#include <memory>

class ICommandHandler {
public:
    virtual ~ICommandHandler() = default;
    // JSONからCommandを生成
    virtual std::unique_ptr<Command> parse(const nlohmann::json& node) = 0;
};