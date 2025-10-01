#include "Input.h"
#include "Circle.h"
#include "Paddle.h"
#include <GLFW/glfw3.h>

void processInput(GLFWwindow* window,
    std::vector<Circle>& world,
    Paddle& paddle,
    bool& requestLaunch,
    bool& paused)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) paddle.moveLeft();
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) paddle.moveRight();

    // launch ball
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) requestLaunch = true;

    // pause toggle (P to pause, O to unpause)
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) paused = true;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) paused = false;
}
