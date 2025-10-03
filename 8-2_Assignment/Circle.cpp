#include "PlatformFixes.h"
#include "Circle.h"
#include "Brick.h"
#include "Paddle.h"
#include "GameMath.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <math.h>

static inline float deg2rad(float d) { return d * 3.1415926535f / 180.0f; }

Circle::Circle(float cx, float cy, float rad, float dirDeg, float speed, float r, float g, float b)
{
    transform.position = { cx, cy };
    radius = rad;
    speed_ = speed;
    color = { r,g,b };
    Vec2 dir{ cosf(deg2rad(dirDeg)), sinf(deg2rad(dirDeg)) };
    transform.velocity = normalize(dir) * speed_;
}

void Circle::attachToPaddle(const Paddle& p) {
    attached = true;
    transform.position = { p.x(), p.y() + p.height() * 0.5f + radius + 0.005f };
    transform.velocity = { 0,0 };
}
void Circle::launchUp() {
    attached = false;
    transform.velocity = normalize(Vec2{ 0.5f, 1.0f }) * speed_;
}

void Circle::integrate(float dt) {
    if (attached) return;
    transform.position += transform.velocity * dt;
    if (transform.position.x - radius < -1.f || transform.position.x + radius > 1.f)
        transform.velocity.x = -transform.velocity.x;
    if (transform.position.y + radius > 1.f)
        transform.velocity.y = -transform.velocity.y;
}

void Circle::draw() const {
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; ++i) {
        float a = deg2rad((float)i);
        glVertex2f(transform.position.x + cosf(a) * radius,
            transform.position.y + sinf(a) * radius);
    }
    glEnd();
}

void Circle::bounceOffNormal(const Vec2& n) {
    float d = dot(transform.velocity, n);
    transform.velocity = transform.velocity - (n * (2.0f * d));
}

void Circle::maybeBounceOffPaddle(const Paddle& p) {
    float hx = p.width() * 0.5f, hy = p.height() * 0.5f;
    Vec2 c = transform.position;
    Vec2 pc{ p.x(), p.y() };
    Vec2 clamped{ clampf(c.x, pc.x - hx, pc.x + hx),
                  clampf(c.y, pc.y - hy, pc.y + hy) };
    Vec2 diff = { c.x - clamped.x, c.y - clamped.y };
    if (dot(diff, diff) <= radius * radius && transform.velocity.y < 0.0f) {
        float rel = clampf((c.x - p.x()) / hx, -1.0f, 1.0f);
        float ang = 90.0f + rel * 60.0f;
        Vec2 dir{ cosf(deg2rad(ang)), sinf(deg2rad(ang)) };
        transform.velocity = normalize(dir) * speed_;
        transform.position.y = p.y() + hy + radius + 0.002f;
    }
}

bool Circle::collideAndResolve(Brick& brick, float dt, size_t& checkCounter) {
    if (brick.state() == ONOFF::OFF) return false;

    Vec2 p0 = transform.position;
    Vec2 p1 = { transform.position.x + transform.velocity.x * dt,
                transform.position.y + transform.velocity.y * dt };
    Vec2 bCenter = brick.transform.position;
    Vec2 bHalf = { brick.half.x + radius, brick.half.y + radius };
    ++checkCounter;

    float tHit;
    Vec2 n;
    if (!segmentIntersectsAABB(p0, p1, bCenter, bHalf, &tHit, &n)) return false;

    transform.position = { p0.x + (p1.x - p0.x) * tHit,
                           p0.y + (p1.y - p0.y) * tHit };
    if (brick.type() == BRICKTYPE::REFLECTIVE) {
        bounceOffNormal(n);
    }
    else {
        brick.hit();
        bounceOffNormal(n);
        speed_ = clampf(speed_ * 1.01f, 0.0f, 1.2f);
        transform.velocity = normalize(transform.velocity) * speed_;
    }
    transform.position += transform.velocity * (dt * 0.2f);
    return true;
}
