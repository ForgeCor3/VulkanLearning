#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <iostream>
#include <vector>

#include "GlobUtils.h"
#include "Window.h"
#include "Vulkan/VulkanInstance.h"
#include "Vulkan/VulkanDebugUtilsMessenger.h"
#include "Vulkan/VulkanDevice.h"

class Application final
{
private:
    DISABLE_COPY_AND_MOVE(Application)

    std::unique_ptr<Window> window;
    std::unique_ptr<VulkanInstance> instance;
    std::unique_ptr<VulkanDebugUtilsMessenger> debugUtilsMessenger;
    std::unique_ptr<VulkanDevice> device;

    std::vector<const char*> validationLayers;

public:
    Application() = delete;
    Application(const WindowConfig& windowConfig, bool validationLayersEnabled);

    ~Application();

    void run();
    void mainloop();
    void terminate();
};

#endif // APPLICATION_H