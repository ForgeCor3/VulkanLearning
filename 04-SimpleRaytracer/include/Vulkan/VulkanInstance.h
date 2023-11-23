#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H

#include <stdexcept>
#include <vector>
#include <memory>

#include "GlobUtils.h"
#include "VulkanUtils.h"
#include "Vulkan/VulkanDebugUtilsMessenger.h"
#include "Window.h"

class VulkanInstance final
{
private:
    DISABLE_COPY_AND_MOVE(VulkanInstance)

    void checkValidationLayerSupport(std::vector<const char*>& validationLayers);

    VkInstance instance = VK_NULL_HANDLE;
    std::unique_ptr<VulkanDebugUtilsMessenger> debugUtilsMessenger;

public:
    VulkanInstance() = delete;
    VulkanInstance(const class Window& window, std::vector<const char*>& validationLayers);
    
    ~VulkanInstance();

    VkInstance& getInstance() { return instance; }
};

#endif // VULKANINSTANCE_H