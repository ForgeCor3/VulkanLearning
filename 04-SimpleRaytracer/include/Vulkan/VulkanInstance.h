#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H

#include "Window.h"

#include <stdexcept>
#include <vector>

class VulkanInstance final
{
private:
    VkInstance instance;
    std::vector<VkExtensionProperties> extensions;

public:
    VulkanInstance() = delete;
    VulkanInstance(const class Window& window);
    
    ~VulkanInstance() { vkDestroyInstance(instance, nullptr); }
};

#endif // VULKANINSTANCE_H