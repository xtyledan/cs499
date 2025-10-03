#include "PlatformFixes.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <chrono>
#include <iostream>
#include <algorithm>

#include "GameMath.h"
#include "Grid.h"
#include "Brick.h"
#include "Paddle.h"
#include "Circle.h"
#include "Collision.h"
#include "Input.h"
#include "PowerUp.h"
#include "Level.h"

namespace {
    float deltaSeconds() {
        static double last = glfwGetTime();
        const double now = glfwGetTime();
        const float dt = static_cast<float>(now - last);
        last = now;
        return dt;
    }

    void drawLives(int lives) {
        for (int i = 0; i < lives; ++i) {
            const float x = -0.98f + i * 0.06f;
            const float y = 0.95f;
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
    // --- init window ---
    if (!glfwInit()) return EXIT_FAILURE;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Brick Breaker - Optimized", 0, 0);
    if (!window) { glfwTerminate(); return EXIT_FAILURE; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // --- game state ---
    std::vector<Circle>  balls;
    std::vector<Brick>   bricks;
    std::vector<PowerUp> powerups;
    Paddle paddle(0.f, -0.85f, 0.30f, 0.05f);
    int   lives = 3;
    bool  requestLaunch = false;
    bool  paused = false;

    buildLevel(bricks);

    // start with one ball attached to paddle
    balls.emplace_back(0.f, 0.f, 0.05f, 90.f, 0.70f, 1.f, 1.f, 1.f);
    balls.front().attachToPaddle(paddle);

    UniformGrid grid(16, 16);

    // --- main loop ---
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // input
        processInput(window, balls, paddle, requestLaunch, paused);
        float dt = deltaSeconds();
        if (paused) dt = 0.0f;

        // rebuild spatial grid
        grid.clear();
        for (int i = 0; i < static_cast<int>(bricks.size()); ++i) {
            if (bricks[i].state() == ONOFF::ON) {
                grid.insert(i, bricks[i].transform.position, bricks[i].half);
            }
        }

        // launch if requested
        if (requestLaunch && !balls.empty() && balls.front().attached) {
            balls.front().launchUp();
        }
        requestLaunch = false;

        // keep attached balls riding the paddle
        for (size_t i = 0; i < balls.size(); ++i) {
            if (balls[i].attached) balls[i].attachToPaddle(paddle);
        }

        // update balls
        size_t collisionChecks = 0;
        for (std::vector<Circle>::iterator it = balls.begin(); it != balls.end(); ) {
            Circle& b = *it;

            // paddle response
            b.maybeBounceOffPaddle(paddle);

            // candidate bricks from grid
            std::vector<int> candidates;
            grid.query(b.transform.position, Vec2{ b.radius, b.radius }, candidates);

            for (size_t k = 0; k < candidates.size(); ++k) {
                const int idx = candidates[k];
                if (idx >= 0 && idx < static_cast<int>(bricks.size())) {
                    b.collideAndResolve(bricks[idx], dt, collisionChecks);
                }
            }

            // integrate
            b.integrate(dt);

            // fell below bottom?
            if (b.transform.position.y - b.radius < -1.f) {
                lives--;
                if (lives <= 0) {
                    lives = 3;
                    powerups.clear();
                    buildLevel(bricks);
                }
                balls.clear();
                balls.emplace_back(0.f, 0.f, 0.05f, 90.f, 0.70f, 1.f, 1.f, 1.f);
                balls.front().attachToPaddle(paddle);
                break; // restart processing after reset
            }
            else {
                ++it;
            }

            // draw the ball
            b.draw();
        }

        // level cleared? (any destructible left ON)
        bool anyDestructible = false;
        for (size_t i = 0; i < bricks.size(); ++i) {
            if (bricks[i].type() == BRICKTYPE::DESTRUCTABLE &&
                bricks[i].state() == ONOFF::ON) {
                anyDestructible = true;
                break;
            }
        }
        if (!anyDestructible) {
            buildLevel(bricks);
            for (size_t i = 0; i < balls.size(); ++i)
                if (!balls[i].attached) balls[i].launchUp();
        }

        // powerups
        for (size_t i = 0; i < powerups.size(); ++i) { powerups[i].update(dt); powerups[i].render(); }
        powerups.erase(
            std::remove_if(powerups.begin(), powerups.end(),
                [](const PowerUp& p) { return !p.active; }),
            powerups.end());

        // draw bricks and paddle
        for (size_t i = 0; i < bricks.size(); ++i) bricks[i].drawBrick();
        paddle.drawPaddle();
        drawLives(lives);

        // (optional) simple perf log
        // std::cout << "collision checks: " << collisionChecks << "\n";

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
