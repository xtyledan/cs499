#pragma once
#include <vector>
#include "Circle.h"

class Paddle;
struct PowerUp;

void CheckCircleCollision(std::vector<Circle>& world);

bool intersects(float ax, float ay, float aw, float ah,
    float bx, float by, float bw, float bh);

// NOTE: keep this exact casing to match Collision.cpp
bool powerUpCaughtByPaddle(const PowerUp& p, const Paddle& pad);
