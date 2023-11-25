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

    if(!validationLayers.empty())
        debugUtilsMessenger.reset(new VulkanDebugUtilsMessenger(instance.get()->getInstance()));

    surface.reset(new VulkanSurface(instance.get()));
    device.reset(new VulkanDevice(instance.get()->getInstance()));
}

Application::~Application()
{
    device.reset();
    
    if(!validationLayers.empty()) debugUtilsMessenger.reset();
    instance.reset();
    window.reset();

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