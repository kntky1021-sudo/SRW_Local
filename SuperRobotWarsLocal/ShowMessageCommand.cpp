#include "ShowMessageCommand.h"
#include "ExecutionEngine.h"
#include "UIManager.h"
#include "InputManager.h"
#include <iostream>

ShowMessageCommand::ShowMessageCommand(const nlohmann::json& evt)
    : text_("")
{
    if (evt.contains("text") && evt["text"].is_string()) {
        text_ = evt["text"].get<std::string>();
    }
    else {
        std::cerr << "[ShowMessageCommand] missing or wrong type for \"text\"\n";
    }
}

void ShowMessageCommand::execute(ExecutionEngine& engine) {
    if (engine.ui) {
        engine.ui->showMessage(text_);
    }
    if (engine.input) {
        engine.input->waitKey();
    }
}