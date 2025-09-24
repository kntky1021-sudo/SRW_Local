#include "Parser.h"
#include "ShowMessageCommand.h"
#include "ChoiceCommand.h"
#include "BattleStartCommand.h"
#include <iostream>

std::vector<std::unique_ptr<Command>> Parser::parse(const nlohmann::json& script) {
    std::vector<std::unique_ptr<Command>> commands;
    if (!script.is_array()) {
        std::cerr << "[Parser] script is not an array\n";
        return commands;
    }

    for (const auto& evt : script) {
        if (!evt.is_object()) continue;
        auto typeIt = evt.find("type");
        if (typeIt == evt.end() || !typeIt->is_string()) {
            std::cerr << "[Parser] missing or wrong type for \"type\"\n";
            continue;
        }
        const std::string type = typeIt->get<std::string>();
        if (type == "showMessage") {
            commands.push_back(std::make_unique<ShowMessageCommand>(evt));
        }
        else if (type == "choice") {
            commands.push_back(std::make_unique<ChoiceCommand>(evt));
        }
        else if (type == "battleStart") {
            commands.push_back(std::make_unique<BattleStartCommand>(evt));
        }
        else {
            std::cerr << "[Parser] unknown command type: " << type << "\n";
        }
    }
    return commands;
}