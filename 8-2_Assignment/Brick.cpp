#include "Brick.h"
#include <GLFW/glfw3.h>

Brick::Brick(BRICKTYPE bt, float xx, float yy, float ww, float hh,
    float rr, float gg, float bb, int hits)
    : red_(rr), green_(gg), blue_(bb),
    x_(xx), y_(yy), width_(ww), height_(hh),
    brick_type_(bt), onoff_(ONOFF::ON), hitCount_(hits) {
}

void Brick::drawBrick() const {
    if (onoff_ == ONOFF::ON) {
        glColor3d(red_, green_, blue_);
        glBegin(GL_POLYGON);
        glVertex2d(x_ + width_ / 2, y_ + height_ / 2);
        glVertex2d(x_ + width_ / 2, y_ - height_ / 2);
        glVertex2d(x_ - width_ / 2, y_ - height_ / 2);
        glVertex2d(x_ - width_ / 2, y_ + height_ / 2);
        glEnd();
    }
}

void Brick::hit() {
    --hitCount_;
    red_ *= 0.8f; green_ *= 0.8f; blue_ *= 0.8f; // Darken color on hit
    if (hitCount_ <= 0) onoff_ = ONOFF::OFF;
}
