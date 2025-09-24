#pragma once

#include "Command.h"
#include <nlohmann/json.hpp>

/**
 * WaitCommand
 * JSON の "duration"（ミリ秒）だけ処理を一時停止します。
 */
class WaitCommand : public Command {
public:
    explicit WaitCommand(const nlohmann::json& evt);
    void execute(ExecutionEngine& engine) override;

private:
    int durationMs_;
};