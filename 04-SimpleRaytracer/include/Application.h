#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <iostream>
#include <vector>

#include "GlobUtils.h"
#include "Window.h"
#include "Vulkan/VulkanInstance.h"
#include "Vulkan/VulkanDebugUtilsMessenger.h"
#include "Vulkan/VulkanSurface.h"
#include "Vulkan/VulkanDevice.h"

class Application final
{
public:
    DISABLE_COPY_AND_MOVE(Application)

    Application() = delete;
    Application(const WindowConfig& windowConfig, bool validationLayersEnabled);
    ~Application();

    void run();
    void mainloop();
    void terminate();

private:
    std::unique_ptr<Window> window;
    std::unique_ptr<VulkanInstance> instance;
    std::unique_ptr<VulkanDebugUtilsMessenger> debugUtilsMessenger;
    std::unique_ptr<VulkanDevice> device;
    std::unique_ptr<VulkanSurface> surface;

    std::vector<const char*> validationLayers;
};

#endif // APPLICATION_H