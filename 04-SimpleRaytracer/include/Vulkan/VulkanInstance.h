#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H

#include "VulkanUtils.h"
#include "Window.h"

#include <stdexcept>
#include <vector>

class VulkanInstance final
{
private:
    void checkValidationLayerSupport(std::vector<const char*>& validationLayers);

    VkInstance instance;
    std::vector<VkExtensionProperties> extensions;
    std::vector<VkLayerProperties> layers;

public:
    VulkanInstance() = delete;
    VulkanInstance(const class Window& window, std::vector<const char*>& validationLayers);
    
    ~VulkanInstance() { vkDestroyInstance(instance, nullptr); }

    VkInstance& getInstance() { return instance; }
};

#endif // VULKANINSTANCE_H