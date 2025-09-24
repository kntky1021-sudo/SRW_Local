#pragma once

/// マップ全体(pixel)とウィンドウサイズ(pixel)からオフセットを計算し、
/// 指定ターゲット座標を中央に据えつつスクロールします。
class Camera {
public:
    /// @param mapW マップ幅(pixel)
    /// @param mapH マップ高さ(pixel)
    /// @param viewW ウィンドウ幅(pixel)
    /// @param viewH ウィンドウ高さ(pixel)
    Camera(int mapW, int mapH, int viewW, int viewH);

    /// 中心にしたい座標(targetX,targetY)を渡し、offsetX/Yを更新
    void update(int targetX, int targetY);

    int getOffsetX() const { return offsetX_; }
    int getOffsetY() const { return offsetY_; }

private:
    int mapW_, mapH_;
    int viewW_, viewH_;
    int offsetX_, offsetY_;
};