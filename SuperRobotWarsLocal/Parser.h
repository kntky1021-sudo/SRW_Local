#pragma once

#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "Command.h"

class ShowMessageCommand;
class ChoiceCommand;
class BattleStartCommand;

class Parser {
public:
    std::vector<std::unique_ptr<Command>> parse(const nlohmann::json& script);
};