#include "PlatformFixes.h"
#include "Brick.h"
#include <GLFW/glfw3.h>

Brick::Brick(BRICKTYPE bt, float cx, float cy, float w, float h,
    float rr, float gg, float bb, int hits)
{
    brick_type_ = bt;
    transform.position = { cx, cy };
    half = { w * 0.5f, h * 0.5f };
    rgb = { rr, gg, bb };
    health.hp = hits;
    state_ = ONOFF::ON;
}

void Brick::drawBrick() const {
    if (state_ == ONOFF::OFF) return;
    glColor3f(rgb[0], rgb[1], rgb[2]);
    glBegin(GL_POLYGON);
    glVertex2f(x() + half.x, y() + half.y);
    glVertex2f(x() + half.x, y() - half.y);
    glVertex2f(x() - half.x, y() - half.y);
    glVertex2f(x() - half.x, y() + half.y);
    glEnd();
}

void Brick::hit() {
    if (state_ == ONOFF::OFF) return;
    if (brick_type_ == BRICKTYPE::DESTRUCTABLE) {
        --health.hp;
        for (auto& c : rgb) c *= 0.85f;
        if (health.hp <= 0) state_ = ONOFF::OFF;
    }
}
