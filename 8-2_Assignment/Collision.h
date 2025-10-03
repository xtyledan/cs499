#pragma once
#include <vector>
#include "Circle.h"   // required (std::vector<Circle> by value)

class Paddle;
struct PowerUp;       // <-- change from 'class PowerUp;' to 'struct PowerUp;'

void CheckCircleCollision(std::vector<Circle>& world);

bool intersects(float ax, float ay, float aw, float ah,
    float bx, float by, float bw, float bh);

bool powerUpCaughtByPaddle(const PowerUp& p, const Paddle& pad);
