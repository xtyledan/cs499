#pragma once
#include <GLFW/glfw3.h>

enum class PowerUpType { ExpandPaddle, MultiBall };

struct PowerUp {
    PowerUpType type{ PowerUpType::ExpandPaddle };  // initialize!
    float x{ 0.f }, y{ 0.f };
    float w{ 0.06f }, h{ 0.04f };
    float vy{ -0.4f };        // falling speed (NDC units/sec)
    bool active{ true };

    void update(float dt) { y += vy * dt; if (y < -1.2f) active = false; }

    void render() const {
        if (!active) return;
        switch (type) {
        case PowerUpType::ExpandPaddle: glColor3f(0.f, 1.f, 0.f); break;   // green
        case PowerUpType::MultiBall:    glColor3f(1.f, 0.5f, 0.f); break; // orange
        }
        glBegin(GL_POLYGON);
        glVertex2f(x + w / 2, y + h / 2);
        glVertex2f(x + w / 2, y - h / 2);
        glVertex2f(x - w / 2, y - h / 2);
        glVertex2f(x - w / 2, y + h / 2);
        glEnd();
    }
};
