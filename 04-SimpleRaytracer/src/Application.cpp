#include "Application.h"

void ErrorCallback(int, const char* err_str)
{
    std::cout << "GLFW Error: " << err_str << std::endl;
}

Application::Application(const WindowConfig& windowConfig)
{
    glfwSetErrorCallback(ErrorCallback);
    window.reset(new Window(windowConfig));
    instance.reset(new VulkanInstance(*window));
}

void Application::run()
{
    mainloop();
}

void Application::mainloop()
{
    while (!glfwWindowShouldClose(window.get()->getWindow()))
    {
        glfwPollEvents();
    }
    terminate();
}

void Application::terminate()
{
    glfwTerminate();
}