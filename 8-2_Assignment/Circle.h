#pragma once
#include "GameMath.h"

class Brick;
class Paddle;

class Circle {
public:
    Circle(float cx, float cy, float rad, float dirDeg, float speed,
        float r, float g, float b);

    // Modern API
    void attachToPaddle(const Paddle& p);
    void launchUp();
    void integrate(float dt);
    void draw() const;
    bool collideAndResolve(Brick& brick, float dt, size_t& checkCounter);
    void bounceOffNormal(const Vec2& n);
    void maybeBounceOffPaddle(const Paddle& p);

    // Accessors (for code that used c.x / c.y)
    float x() const { return transform.position.x; }
    float y() const { return transform.position.y; }

    // ---- Legacy wrappers so old code compiles unchanged ----
    // (Capitals and names match old calls in MainCode.cpp)
    void AttachToPaddle(const Paddle& p) { attachToPaddle(p); }
    void LaunchUp() { launchUp(); }
    void MoveOneStep(float dt) { integrate(dt); }
    void BounceOffPaddle(const Paddle& p) { maybeBounceOffPaddle(p); }
    void DrawCircle() const { draw(); }

    // --------------------------------------------------------

    // Data
    Transform transform;    // position + velocity
    float radius{ 0.05f };
    struct { float r{ 1 }, g{ 1 }, b{ 1 }; } color;
    bool attached{ true };

private:
    float speed_{ 0.012f };
};
