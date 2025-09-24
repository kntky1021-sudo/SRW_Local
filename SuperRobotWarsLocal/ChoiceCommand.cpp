#include "ChoiceCommand.h"
#include "ExecutionEngine.h"
#include "UIManager.h"
#include "InputManager.h"
#include <iostream>

ChoiceCommand::ChoiceCommand(const nlohmann::json& evt)
    : prompt_(""), options_(), resultLabels_()
{
    // …省略…
}

void ChoiceCommand::execute(ExecutionEngine& engine) {
    if (!engine.ui || !engine.input) {
        std::cerr << "[ChoiceCommand] ui or input not set\n";
        return;
    }

    // 1) プロンプトを表示してキー待ち
    engine.ui->showMessage(prompt_);
    engine.input->waitKey();

    // 2) 選択肢を表示して入力待ち
    engine.ui->promptChoice(options_);
    int choice = engine.input->waitForChoice(static_cast<int>(options_.size()));
    if (choice < 0 || choice >= static_cast<int>(resultLabels_.size())) {
        choice = 0;
    }

    // 3) 結果ラベルを表示→キー待ち
    engine.ui->showMessage(resultLabels_[choice]);
    engine.input->waitKey();

    // 4) ダイアログ終了後にゲーム画面を再描画
    engine.redraw();
}