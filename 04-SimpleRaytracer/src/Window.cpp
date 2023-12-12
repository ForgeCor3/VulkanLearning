#include "Window.h"

void error_callback(int code, const char* err_str) { std::cerr << "GLFW Error [" << code << "]: " << err_str << std::endl; }

Window::Window(const WindowConfig windowConfig)
{
    glfwSetErrorCallback(error_callback);

    if(!glfwInit())
        throw std::runtime_error("Failed to initialize glfw.");

    this->windowConfig = windowConfig;

    GLFWmonitor* isFullscreen = windowConfig.isFullscreen ? glfwGetPrimaryMonitor() : nullptr;

    glfwWindowHint(GLFW_RESIZABLE, this->windowConfig.isResizable);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    this->window = glfwCreateWindow(
            this->windowConfig.windowWidth,
            this->windowConfig.windowHeight,
            this->windowConfig.windowTitle.c_str(),
            isFullscreen, nullptr);
    
    if(window == nullptr)
        throw std::runtime_error("Failed to create window.");
}

Window::~Window() { glfwDestroyWindow(window); }

GLFWwindow* Window::getWindow() const  { return window; }

std::vector<const char*> Window::getRequiredInstanceExtensions() const
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    return {glfwExtensions, glfwExtensions + glfwExtensionCount};
}