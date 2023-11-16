#ifndef WINDOW_H
#define WINDOW_H

#include "WindowConfig.h"

#include <GLFW/glfw3.h>
#include <stdexcept>

class Window final
{
private:
    GLFWwindow* window;
    WindowConfig windowConfig;

public:
    Window() = delete;
    Window(const WindowConfig windowConfig);

    ~Window() {};

    GLFWwindow* getWindow() { return this->window; };

    void terminate() { glfwDestroyWindow(window); };
};

#endif // WINDOW_H