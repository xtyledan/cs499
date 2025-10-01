#include "Paddle.h"
#include <GLFW/glfw3.h>

Paddle::Paddle(float xx, float yy, float ww, float hh)
    : x_(xx), y_(yy), width_(ww), height_(hh) {
}

void Paddle::moveLeft() { if (x_ - width_ / 2 > -1) x_ -= speed_; }
void Paddle::moveRight() { if (x_ + width_ / 2 < 1) x_ += speed_; }

void Paddle::drawPaddle() const {
    glColor3f(1.f, 1.f, 1.f);
    glBegin(GL_POLYGON);
    glVertex2d(x_ + width_ / 2, y_ + height_ / 2);
    glVertex2d(x_ + width_ / 2, y_ - height_ / 2);
    glVertex2d(x_ - width_ / 2, y_ - height_ / 2);
    glVertex2d(x_ - width_ / 2, y_ + height_ / 2);
    glEnd();
}
