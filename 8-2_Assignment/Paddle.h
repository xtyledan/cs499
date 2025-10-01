#pragma once

class Paddle {
public:
    Paddle(float xx, float yy, float ww, float hh);

    void moveLeft();
    void moveRight();
    void drawPaddle() const;

    float x() const { return x_; }
    float y() const { return y_; }
    float width()  const { return width_; }
    float height() const { return height_; }
    void expand(float factor) { width_ = (width_ * factor > 0.8f) ? 0.8f : width_ * factor; }
    void shrink(float factor) { width_ = (width_ * factor < 0.1f) ? 0.1f : width_ * factor; }

private:
    float x_, y_, width_, height_;
    float speed_ = 0.05f;
};
