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
    // 1) テキストを UI レイヤーに描画（修正：getUI()を使用）
    if (engine.getUI()) {
        engine.getUI()->showMessage(text_);
    }
    // 2) キー入力で待機（修正：getInput()を使用）
    if (engine.getInput()) {
        engine.getInput()->waitKey();
    }
    // 3) ダイアログ終了後に必ずゲーム画面を再描画
    engine.redraw();
}