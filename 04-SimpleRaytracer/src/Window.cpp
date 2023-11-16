#include "Window.h"

Window::Window(const WindowConfig windowConfig)
{
    if(!glfwInit())
        throw std::runtime_error("Failed to initialize glfw.");

    this->windowConfig = windowConfig;

    glfwWindowHint(GLFW_RESIZABLE, this->windowConfig.isResizable);

    GLFWmonitor* isFullscreen = windowConfig.isFullscreen ? glfwGetPrimaryMonitor() : nullptr;

    this->window = glfwCreateWindow(
            this->windowConfig.windowWidth,
            this->windowConfig.windowHeight,
            this->windowConfig.windowTitle.c_str(),
            isFullscreen, nullptr);
    
    if(window == nullptr)
        throw std::runtime_error("Failed to create window.");
}