#include "Circle.h"
#include "Brick.h"
#include "Paddle.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <algorithm>

namespace { constexpr float DEG2RAD = 3.14159f / 180.f; }

template <typename T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}
Circle::Circle(double xx, double yy, float rad, float dir, float spd,
    float r, float g, float b)
    : x(static_cast<float>(xx)), y(static_cast<float>(yy)),
    radius(rad), speed(spd), direction(dir),
    red(r), green(g), blue(b) {
}

void Circle::AttachToPaddle(const Paddle& p) {
    attachedToPaddle = true;
    // sit on top of paddle center
    x = p.x();
    y = p.y() + p.height() / 2.f + radius + 0.005f;
    direction = 90.f; // pointing up
}

void Circle::LaunchUp() {
    attachedToPaddle = false;
    // small upward angle to start
    direction = 75.f;
}

void Circle::BounceOffPaddle(const Paddle& p) {
    // reflect based on where the ball hits the paddle
    float rel = clamp((x - p.x()) / (p.width() / 2.f), -1.f, 1.f);
    // map [-1,1] to [30°,150°] (steeper angles at edges)
    direction = 90.f + rel * 60.f;
    // slight speed-up each hit
    speed = std::min(speed * 1.03f, 1.2f);
}

void Circle::CheckCollision(Brick* brk) {
    if (!brk) return;
    if (brk->onoff() == ONOFF::ON &&
        x > brk->x() - brk->width() / 2 && x < brk->x() + brk->width() / 2 &&
        y > brk->y() - brk->height() / 2 && y < brk->y() + brk->height() / 2)
    {
        if (brk->type() == BRICKTYPE::REFLECTIVE) {
            direction = 180.f - direction;
        }
        else if (brk->type() == BRICKTYPE::DESTRUCTABLE) {
            brk->hit();
            direction = -direction;       // bounce back after hit
            speed = std::min(speed * 1.01f, 1.2f);
        }
    }
}

void Circle::MoveOneStep() {
    if (attachedToPaddle) return;

    x += speed * std::cos(direction * DEG2RAD);
    y += speed * std::sin(direction * DEG2RAD);

    // Bounce off side/top walls with friction
    if (x - radius < -1 || x + radius > 1) {
        direction = 180.f - direction;
        speed *= 0.99f;
    }
    if (y + radius > 1) {
        direction = -direction;
        speed *= 0.99f;
    }
}

void Circle::DrawCircle() const {
    glColor3f(red, green, blue);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float degInRad = i * DEG2RAD;
        glVertex2f((std::cos(degInRad) * radius) + x,
            (std::sin(degInRad) * radius) + y);
    }
    glEnd();
}
