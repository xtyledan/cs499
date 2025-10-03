#include "Collision.h"
#include "Circle.h"
#include "Paddle.h"
#include "PowerUp.h"
#include <cmath>

void CheckCircleCollision(std::vector<Circle>& world) {
    for (size_t i = 0; i < world.size(); i++) {
        for (size_t j = i + 1; j < world.size(); j++) {
            float dx = world[i].x - world[j].x;
            float dy = world[i].y - world[j].y;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance < world[i].radius + world[j].radius) {
                world.erase(world.begin() + j);
                world.erase(world.begin() + i);
                return;
            }
        }
    }
}

bool intersects(float ax, float ay, float aw, float ah,
    float bx, float by, float bw, float bh)
{
    return std::abs(ax - bx) * 2.f < (aw + bw) &&
        std::abs(ay - by) * 2.f < (ah + bh);
}

bool powerUpCaughtByPaddle(const PowerUp& p, const Paddle& pad) {
    return intersects(p.x, p.y, p.w, p.h, pad.x(), pad.y(), pad.width(), pad.height());
}
