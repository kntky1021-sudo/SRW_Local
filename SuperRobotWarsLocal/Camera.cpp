#include "Camera.h"
#include <algorithm>

Camera::Camera(int mapW, int mapH, int viewW, int viewH)
    : mapW_(mapW)
    , mapH_(mapH)
    , viewW_(viewW)
    , viewH_(viewH)
    , offsetX_(0)
    , offsetY_(0)
{
}

void Camera::update(int targetX, int targetY) {
    // ターゲットをウィンドウ中央に寄せる
    int cx = targetX - viewW_ / 2;
    int cy = targetY - viewH_ / 2;

    // マップ範囲内にクランプ
    offsetX_ = std::clamp(cx, 0, std::max(0, mapW_ - viewW_));
    offsetY_ = std::clamp(cy, 0, std::max(0, mapH_ - viewH_));
}