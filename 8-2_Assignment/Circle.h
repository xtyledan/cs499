#pragma once

class Brick;
class Paddle;

class Circle {
public:
    Circle(double xx, double yy, float rad, float dir, float spd,
        float r, float g, float b);

    void CheckCollision(Brick* brk);
    void MoveOneStep();
    void DrawCircle() const;

    // New: paddle interaction + launch logic
    void AttachToPaddle(const Paddle& p);
    void LaunchUp();                // called when space pressed while attached
    void BounceOffPaddle(const Paddle& p);

    // public position for simple checks
    float x, y, radius;

    // state
    bool attachedToPaddle{ true };

private:
    float red, green, blue;
    float speed;
    float direction; // Angle in degrees
};
