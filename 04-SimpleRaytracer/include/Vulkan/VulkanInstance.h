#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>

#include "Window.h"

class VulkanInstance final
{
private:
    VkInstance instance;
    std::vector<VkExtensionProperties> extensions;

public:
    VulkanInstance() = delete;
    VulkanInstance(const class Window& window);
    
    ~VulkanInstance() { };

    void terminate();
};

#endif // VULKANINSTANCE_H