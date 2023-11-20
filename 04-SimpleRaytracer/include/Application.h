#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <iostream>
#include <vector>

#include "Window.h"
#include "Vulkan/VulkanInstance.h"
#include "Vulkan/VulkanDebugUtilsMessenger.h"

class Application final
{
private:
    std::unique_ptr<Window> window;
    std::unique_ptr<VulkanInstance> instance;
    std::unique_ptr<VulkanDebugUtilsMessenger> debugUtilsMessenger;

    std::vector<const char*> validationLayers;

public:
    Application() = delete;
    Application(const WindowConfig& windowConfig, bool validationLayersEnabled);

    ~Application() { };

    void run();
    void mainloop();
    void terminate();
};

#endif // APPLICATION_H