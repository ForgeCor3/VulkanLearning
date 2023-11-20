#include "Application.h"

void error_callback(int, const char* err_str) { std::cout << "GLFW Error: " << err_str << std::endl; }

Application::Application(const WindowConfig& windowConfig, bool validationLayersEnabled)
{
    glfwSetErrorCallback(error_callback);

    validationLayers = validationLayersEnabled ?
        std::vector<const char*>{"VK_LAYER_KHRONOS_validation"} :
        std::vector<const char*>{};

    window.reset(new Window(windowConfig));
    instance.reset(new VulkanInstance(*window, validationLayers));
    debugUtilsMessenger.reset(new VulkanDebugUtilsMessenger(instance.get()->getInstance()));
}

Application::~Application()
{
    debugUtilsMessenger.release();
    instance.release();
    window.release();
    glfwTerminate();
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
}