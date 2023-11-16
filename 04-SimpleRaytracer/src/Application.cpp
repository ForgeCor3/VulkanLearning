#include "Application.h"

Application::Application(const WindowConfig& windowConfig) : window(windowConfig) {}

void Application::run()
{
    mainloop();
}

void Application::mainloop()
{
    while (!glfwWindowShouldClose(window.getWindow()))
    {
        glfwPollEvents();
    }
    terminate();
}

void Application::terminate()
{
    window.terminate();
    glfwTerminate();
}