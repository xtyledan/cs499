// MainCode.cpp
#include <GLFW/glfw3.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>    // for std::clamp
#include <cmath>
#include "Brick.h"
#include "Paddle.h"
#include "Circle.h"
#include "Collision.h"
#include "Input.h"
#include "PowerUp.h"
#include "Level.h"

namespace {
    // simple fixed timestep for power-up falling & bounce feel
    float deltaSeconds() {
        static double last = glfwGetTime();
        double now = glfwGetTime();
        float dt = static_cast<float>(now - last);
        last = now;
        return dt;
    }

    void drawLives(int lives) {
        // draw small squares at top-left as lives
        for (int i = 0; i < lives; ++i) {
            float x = -0.98f + i * 0.06f;
            float y = 0.95f;
            glColor3f(1.f, 0.f, 0.f);
            glBegin(GL_POLYGON);
            glVertex2f(x + 0.02f, y + 0.02f);
            glVertex2f(x + 0.02f, y - 0.02f);
            glVertex2f(x - 0.02f, y - 0.02f);
            glVertex2f(x - 0.02f, y + 0.02f);
            glEnd();
        }
    }
}

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    if (!glfwInit()) return EXIT_FAILURE;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow* window = glfwCreateWindow(480, 480, "Brick Breaker-ish", nullptr, nullptr);
    if (!window) { glfwTerminate(); return EXIT_FAILURE; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // ---- game state ----
    std::vector<Circle> balls;
    std::vector<Brick> bricks;
    std::vector<PowerUp> powerups;
    Paddle paddle(0.f, -0.85f, 0.3f, 0.05f);

    int lives = 3;
    int score = 0;
    bool requestLaunch = false;
    bool paused = false;

    buildLevel(bricks);

    // start with one ball attached to paddle
    balls.emplace_back(0, 0, 0.05f, 90.f, 0.012f, 1.f, 1.f, 1.f);
    balls.front().AttachToPaddle(paddle);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // input
        processInput(window, balls, paddle, requestLaunch, paused);
        float dt = deltaSeconds();
        if (paused) dt = 0.f;

        // launch if requested
        if (requestLaunch && !balls.empty() && balls.front().attachedToPaddle) {
            balls.front().LaunchUp();
        }
        requestLaunch = false;

        // if ball is attached, keep it riding the paddle
        for (auto& b : balls) if (b.attachedToPaddle) b.AttachToPaddle(paddle);

        // update balls
        for (auto& b : balls) {
            // paddle bounce
            if (!b.attachedToPaddle &&
                intersects(b.x, b.y, b.radius * 2.f, b.radius * 2.f,
                    paddle.x(), paddle.y(), paddle.width(), paddle.height()))
            {
                b.BounceOffPaddle(paddle);
                // nudge above paddle to avoid re-collision stickiness
                b.y = paddle.y() + paddle.height() / 2.f + b.radius + 0.002f;
            }

            // brick collisions + scoring + occasional power-ups
            for (auto& brick : bricks) {
                ONOFF prev = brick.onoff();
                b.CheckCollision(&brick);
                if (prev == ONOFF::ON && brick.onoff() == ONOFF::OFF) {
                    score += 10;
                    // 15% chance to spawn a power-up
                    if ((std::rand() % 100) < 15) {
                        PowerUp pu;
                        pu.type = (std::rand() % 2 == 0) ? PowerUpType::ExpandPaddle : PowerUpType::MultiBall;
                        pu.x = brick.x();
                        pu.y = brick.y();
                        powerups.push_back(pu);
                    }
                }
            }

            b.MoveOneStep();

            // bottom-out: lose a life
            if (b.y - b.radius < -1.f) {
                lives--;
                if (lives <= 0) {
                    // reset everything (basic "game over" loop)
                    lives = 3; score = 0; powerups.clear();
                    buildLevel(bricks);
                }
                // reset to one ball attached to paddle
                balls.clear();
                balls.emplace_back(0, 0, 0.05f, 90.f, 0.012f, 1.f, 1.f, 1.f);
                balls.front().AttachToPaddle(paddle);
                break; // leave ball loop after reset
            }

            b.DrawCircle();
        }

        // check if level cleared (all destructible off)
        bool anyDestructible = false;
        for (const auto& br : bricks) {
            if (br.type() == BRICKTYPE::DESTRUCTABLE && br.onoff() == ONOFF::ON) { anyDestructible = true; break; }
        }
        if (!anyDestructible) {
            buildLevel(bricks); // next level (same layout for now)
            // give bonus ball speed-up for fun
            for (auto& b : balls) b.LaunchUp();
        }

        // update/draw power-ups and apply on catch
        for (auto& pu : powerups) {
            pu.update(dt);
            if (pu.active && powerUpCaughtByPaddle(pu, paddle)) {
                pu.active = false;
                if (pu.type == PowerUpType::ExpandPaddle) {
                    paddle.expand(1.3f);
                }
                else if (pu.type == PowerUpType::MultiBall) {
                    if (!balls.empty()) {
                        Circle clone = balls.front();
                        clone.attachedToPaddle = false;
                        clone.LaunchUp();
                        clone.x += 0.05f;
                        clone.y += 0.02f;
                        balls.push_back(clone);
                    }
                }
            }
            pu.render();
        }
        // remove inactive
        powerups.erase(std::remove_if(powerups.begin(), powerups.end(),
            [](const PowerUp& p) { return !p.active; }), powerups.end());

        // render bricks & paddle & HUD
        for (auto& brick : bricks) brick.drawBrick();
        paddle.drawPaddle();
        drawLives(lives);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
