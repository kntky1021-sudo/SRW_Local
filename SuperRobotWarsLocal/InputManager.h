#pragma once
#include <SDL.h>

class InputManager {
public:
    InputManager();

    // メインループで毎フレーム呼ぶ
    void pollEvents();

    // 終了要求を受け取ったか
    bool quitRequested() const;

    // 任意キー押下を待つ
    void waitKey();

    // 0～(maxChoices-1) の数字キー押下を待ち、そのインデックスを返す
    int waitForChoice(int maxChoices);

private:
    bool quitRequested_ = false;
    int  lastChoice_ = -1;
};