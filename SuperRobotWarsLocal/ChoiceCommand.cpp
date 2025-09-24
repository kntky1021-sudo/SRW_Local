#include "ChoiceCommand.h"
#include "ExecutionEngine.h"
#include "UIManager.h"
#include "InputManager.h"
#include <iostream>

ChoiceCommand::ChoiceCommand(const nlohmann::json& evt)
    : prompt_(""), options_(), resultLabels_()
{
    if (evt.contains("prompt") && evt["prompt"].is_string()) {
        prompt_ = evt["prompt"].get<std::string>();
    }
    else {
        std::cerr << "[ChoiceCommand] missing or wrong type for \"prompt\"\n";
    }

    if (evt.contains("options") && evt["options"].is_array()) {
        for (auto& el : evt["options"]) {
            if (el.is_string()) {
                options_.push_back(el.get<std::string>());
            }
        }
    }
    else {
        std::cerr << "[ChoiceCommand] missing or wrong type for \"options\"\n";
    }

    if (evt.contains("resultLabels") && evt["resultLabels"].is_array()) {
        for (auto& el : evt["resultLabels"]) {
            if (el.is_string()) {
                resultLabels_.push_back(el.get<std::string>());
            }
        }
    }
}

void ChoiceCommand::execute(ExecutionEngine& engine) {
    if (!engine.ui || !engine.input) {
        std::cerr << "[ChoiceCommand] ui or input not set\n";
        return;
    }

    engine.ui->showMessage(prompt_);
    engine.input->waitKey();

    engine.ui->promptChoice(options_);
    int choice = engine.input->waitForChoice(static_cast<int>(options_.size()));

    if (choice < 0 || choice >= static_cast<int>(resultLabels_.size())) {
        choice = 0;
    }

    engine.ui->showMessage(resultLabels_[choice]);
    engine.input->waitKey();
}