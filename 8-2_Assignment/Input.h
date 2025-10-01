#pragma once
#include <vector>
#include "Circle.h"

struct GLFWwindow;
class Paddle;

// NOTE: New signature: requestLaunch + paused (both bool&)
void processInput(GLFWwindow* window,
    std::vector<Circle>& world,
    Paddle& paddle,
    bool& requestLaunch,
    bool& paused);
