#pragma once
#include <array>
#include "GameMath.h"   // <-- IMPORTANT: brings Vec2, Transform

enum class BRICKTYPE { REFLECTIVE, DESTRUCTABLE };
enum class ONOFF { ON, OFF };

class Brick {
public:
    Brick(BRICKTYPE bt, float cx, float cy, float w, float h,
        float rr, float gg, float bb, int hits);

    void drawBrick() const;

    // Public data used by the game loop / collision
    Transform transform;     // center position & velocity (velocity unused for bricks)
    Vec2      half{ 0.15f, 0.05f }; // half-extents (w/2, h/2)
    Health    health;

    BRICKTYPE type()  const { return brick_type_; }
    ONOFF     state() const { return state_; }

    float x() const { return transform.position.x; }
    float y() const { return transform.position.y; }
    float w() const { return half.x * 2.0f; }
    float h() const { return half.y * 2.0f; }

    void hit(); // darken & decrement (turn OFF when <=0 for destructibles)

    std::array<float, 3> rgb{ 1,1,1 };

private:
    BRICKTYPE brick_type_{ BRICKTYPE::DESTRUCTABLE };
    ONOFF     state_{ ONOFF::ON };
};
