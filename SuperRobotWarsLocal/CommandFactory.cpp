#include "CommandFactory.h"
#include "ShowMessageCommand.h"
#include "ChoiceCommand.h"
#include "BattleStartCommand.h"
#include <nlohmann/json.hpp>
#include <string>

std::unique_ptr<Command> CommandFactory::create(const nlohmann::json& evt) {
    if (!evt.contains("type") || !evt["type"].is_string()) {
        return nullptr;
    }

    const std::string type = evt["type"].get<std::string>();
    if (type == "showMessage") {
        return std::make_unique<ShowMessageCommand>(evt);
    }
    if (type == "choice") {
        return std::make_unique<ChoiceCommand>(evt);
    }
    if (type == "battleStart") {
        return std::make_unique<BattleStartCommand>(evt);
    }

    // 未知の type は無視
    return nullptr;
}