#pragma once

class ExecutionEngine;

class Command {
public:
    virtual ~Command() = default;
    virtual void execute(ExecutionEngine& engine) = 0;
};