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

private:
    SDLRenderer* renderer_;
    int x_, y_;
    int tileW_, tileH_;
};