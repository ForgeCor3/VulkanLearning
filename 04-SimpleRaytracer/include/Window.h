#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN
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

    ~Window() { glfwDestroyWindow(window); }

    GLFWwindow* getWindow() const;
    std::vector<const char*> getRequiredInstanceExtensions() const;
};

#endif // WINDOW_H