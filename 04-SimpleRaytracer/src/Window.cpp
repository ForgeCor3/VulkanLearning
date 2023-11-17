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

GLFWwindow* Window::getWindow() const  { return this->window; }

std::vector<const char*> Window::getRequiredInstanceExtensions() const
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    return {glfwExtensions, glfwExtensions + glfwExtensionCount};
}