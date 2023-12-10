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
public:
    DISABLE_COPY_AND_MOVE(VulkanInstance)

    VulkanInstance() = delete;
    VulkanInstance(const class Window& window, std::vector<const char*>& validationLayers);
    ~VulkanInstance();

    VkInstance& getInstance();
    GLFWwindow* getWindow();

private:
    void checkValidationLayerSupport(std::vector<const char*>& validationLayers);

    VkInstance instance = VK_NULL_HANDLE;
    std::unique_ptr<VulkanDebugUtilsMessenger> debugUtilsMessenger;
    GLFWwindow* window;
};

#endif // VULKANINSTANCE_H