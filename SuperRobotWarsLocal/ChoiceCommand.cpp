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

    if (evt.contains("options") && evt["options"].is_array()) {
        for (const auto& opt : evt["options"]) {
            if (opt.is_string()) {
                options_.push_back(opt.get<std::string>());
            }
        }
    }

    if (evt.contains("results") && evt["results"].is_array()) {
        for (const auto& res : evt["results"]) {
            if (res.is_string()) {
                resultLabels_.push_back(res.get<std::string>());
            }
        }
    }
}

void ChoiceCommand::execute(ExecutionEngine& engine) {
    auto ui = engine.getUI();
    auto input = engine.getInput();

    if (!ui || !input) {
        std::cerr << "[ChoiceCommand] ui or input not set\n";
        return;
    }

    // 1) プロンプトを表示してキー待ち
    ui->showMessage(prompt_);
    input->waitKey();

    // 2) 選択肢を表示して入力待ち
    ui->promptChoice(options_);
    int choice = input->waitForChoice(static_cast<int>(options_.size()));
    if (choice < 0 || choice >= static_cast<int>(resultLabels_.size())) {
        choice = 0;
    }

    // 3) 結果ラベルを表示→キー待ち
    ui->showMessage(resultLabels_[choice]);
    input->waitKey();

    // 4) ダイアログ終了後にゲーム画面を再描画
    engine.redraw();
}