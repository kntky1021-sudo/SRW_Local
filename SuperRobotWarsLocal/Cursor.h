#pragma once
class SDLRenderer;

class Cursor {
public:
    explicit Cursor(SDLRenderer* renderer, int tileW, int tileH);
    void setPosition(int tx, int ty);
    void move(int dx, int dy, int mapW, int mapH);

    /// @param offsetX,offsetY カメラオフセット(pixel)
    void render(int offsetX, int offsetY) const;

    // playerMove用：カーソル座標取得
    int getX() const { return x_; }
    int getY() const { return y_; }

    // 移動範囲取得（デフォルト値を返す）
    int getMoveRange() const { return 5; }  // デフォルト移動力

private:
    SDLRenderer* renderer_;
    int x_, y_;
    int tileW_, tileH_;
};