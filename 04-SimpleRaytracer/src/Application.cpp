#include "Application.h"

Application::Application(const WindowConfig& windowConfig)
{
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
    instance.get()->terminate();
    window.get()->terminate();
    glfwTerminate();
}