#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>

#include "WindowConfig.h"

class Window final
{
private:
    GLFWwindow* window;
    WindowConfig windowConfig;

public:
    Window() = delete;
    Window(const WindowConfig windowConfig);

    ~Window() {};

    GLFWwindow* getWindow() const;
    std::vector<const char*> getRequiredInstanceExtensions() const;

    void terminate() { glfwDestroyWindow(window); };
};

#endif // WINDOW_H