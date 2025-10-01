#include <GLFW/glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include <time.h>
#include <math.h>

using namespace std;

const float DEG2RAD = 3.14159 / 180;
const double SPAWN_DELAY = 0.5; // 0.5 seconds delay

void processInput(GLFWwindow* window);

enum BRICKTYPE { REFLECTIVE, DESTRUCTABLE };
enum ONOFF { ON, OFF };

double lastSpawnTime = 0.0; // Store last time a circle was spawned

class Brick
{
public:
    float red, green, blue;
    float x, y, width, height;
    BRICKTYPE brick_type;
    ONOFF onoff;
    int hitCount;

    Brick(BRICKTYPE bt, float xx, float yy, float ww, float hh, float rr, float gg, float bb, int hits)
        : hitCount(hits) {
        brick_type = bt; x = xx; y = yy; width = ww; height = hh;
        red = rr; green = gg; blue = bb;
        onoff = ON;
    }

    void drawBrick()
    {
        if (onoff == ON)
        {
            glColor3d(red, green, blue);
            glBegin(GL_POLYGON);
            glVertex2d(x + width / 2, y + height / 2);
            glVertex2d(x + width / 2, y - height / 2);
            glVertex2d(x - width / 2, y - height / 2);
            glVertex2d(x - width / 2, y + height / 2);
            glEnd();
        }
    }

    void hit()
    {
        hitCount--;
        red *= 0.8; green *= 0.8; blue *= 0.8; // Darken color on hit
        if (hitCount <= 0) onoff = OFF;
    }
};

class Paddle {
public:
    float x, y, width, height;
    float speed = 0.05;

    Paddle(float xx, float yy, float ww, float hh) {
        x = xx; y = yy; width = ww; height = hh;
    }

    void moveLeft() {
        if (x - width / 2 > -1) x -= speed;
    }

    void moveRight() {
        if (x + width / 2 < 1) x += speed;
    }

    void drawPaddle() {
        glColor3f(1, 1, 1);
        glBegin(GL_POLYGON);
        glVertex2d(x + width / 2, y + height / 2);
        glVertex2d(x + width / 2, y - height / 2);
        glVertex2d(x - width / 2, y - height / 2);
        glVertex2d(x - width / 2, y + height / 2);
        glEnd();
    }
};

class Circle
{
public:
    float red, green, blue;
    float radius;
    float x, y, speed;
    float direction; // Angle in degrees

    Circle(double xx, double yy, float rad, float dir, float spd, float r, float g, float b)
        : x(xx), y(yy), radius(rad), direction(dir), speed(spd), red(r), green(g), blue(b) {
    }

    void CheckCollision(Brick* brk)
    {
        if (brk->onoff == ON && x > brk->x - brk->width / 2 && x < brk->x + brk->width / 2 &&
            y > brk->y - brk->height / 2 && y < brk->y + brk->height / 2)
        {
            if (brk->brick_type == REFLECTIVE) {
                direction = 180 - direction;
            }
            else if (brk->brick_type == DESTRUCTABLE) {
                brk->hit();
            }
        }
    }

    void MoveOneStep()
    {
        x += speed * cos(direction * DEG2RAD);
        y += speed * sin(direction * DEG2RAD);

        // Bounce off walls with friction
        if (x - radius < -1 || x + radius > 1) {
            direction = 180 - direction;
            speed *= 0.9;
        }
        if (y - radius < -1 || y + radius > 1) {
            direction = -direction;
            speed *= 0.9;
        }
    }

    void DrawCircle()
    {
        glColor3f(red, green, blue);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 360; i++) {
            float degInRad = i * DEG2RAD;
            glVertex2f((cos(degInRad) * radius) + x, (sin(degInRad) * radius) + y);
        }
        glEnd();
    }
};

vector<Circle> world;
vector<Brick> bricks;
Paddle paddle(0, -0.8, 0.3, 0.05);

void CheckCircleCollision(vector<Circle>& world)
{
    for (size_t i = 0; i < world.size(); i++) {
        for (size_t j = i + 1; j < world.size(); j++) {
            float dx = world[i].x - world[j].x;
            float dy = world[i].y - world[j].y;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < world[i].radius + world[j].radius) {
                world.erase(world.begin() + j);
                world.erase(world.begin() + i);
                return;
            }
        }
    }
}

int main(void) {
    srand(time(NULL));

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow* window = glfwCreateWindow(480, 480, "8-2 Assignment", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    bricks = {
        {REFLECTIVE,  0.5,  0.5,  0.3, 0.1, 1, 0, 0, 3},
        {DESTRUCTABLE, -0.5,  0.5,  0.2, 0.1, 0, 1, 0, 2},
        {DESTRUCTABLE,  0.0,  0.3,  0.25, 0.1, 0, 1, 1, 2},
        {REFLECTIVE, -0.5, -0.3,  0.35, 0.1, 1, 1, 0, 2},
    };

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        processInput(window);

        for (auto& circle : world) {
            for (auto& brick : bricks) {
                circle.CheckCollision(&brick);
            }
            circle.MoveOneStep();
            circle.DrawCircle();
        }

        CheckCircleCollision(world);

        for (auto& brick : bricks) {
            brick.drawBrick();
        }
        paddle.drawPaddle();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void processInput(GLFWwindow* window)
{
    double currentTime = glfwGetTime();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && (currentTime - lastSpawnTime) > SPAWN_DELAY)
    {
        world.emplace_back(0, 0, 0.05, rand() % 360, 0.03, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
        lastSpawnTime = currentTime;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        paddle.moveLeft();
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        paddle.moveRight();
}
