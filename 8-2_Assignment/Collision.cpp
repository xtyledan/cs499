#include "PlatformFixes.h"
#include "Collision.h"
#include "Paddle.h"
#include "PowerUp.h"
#include <algorithm>
#include <math.h>   // fabsf

void CheckCircleCollision(std::vector<Circle>& world) {
    // remove two balls if they overlap (simple demo behavior)
    for (size_t i = 0; i < world.size(); ++i) {
        for (size_t j = i + 1; j < world.size(); ++j) {
            Circle& A = world[i];
            Circle& B = world[j];

            const float dx = A.transform.position.x - B.transform.position.x;
            const float dy = A.transform.position.y - B.transform.position.y;
            const float rr = A.radius + B.radius;

            if (dx * dx + dy * dy < rr * rr) {
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
    // AABB vs AABB overlap test
    return fabsf(ax - bx) * 2.f < (aw + bw) &&
        fabsf(ay - by) * 2.f < (ah + bh);
}

bool powerUpCaughtByPaddle(const PowerUp& p, const Paddle& pad) {
    return intersects(p.x, p.y, p.w, p.h,
        pad.x(), pad.y(), pad.width(), pad.height());
}
