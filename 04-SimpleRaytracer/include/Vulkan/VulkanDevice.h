#ifndef VULKANDEVICE_H
#define VULKANDEVICE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <stdexcept>
#include <map>
#include <set>
#include <optional>

#include "GlobUtils.h"
#include "Vulkan/VulkanUtils.h"
#include "Vulkan/VulkanSurface.h"

class VulkanDevice final
{
public:
    DISABLE_COPY_AND_MOVE(VulkanDevice)

    VulkanDevice() = delete;
    VulkanDevice(VulkanInstance& instance, VulkanSurface& surface);
    ~VulkanDevice();

private:
    std::optional<uint32_t> findQueue(const VkQueueFlags queueFlags, const VkPhysicalDevice physicalDevice);

    void selectPhysicalDevice(VulkanInstance& instance);
    VkPhysicalDevice findSuitablePhysicalDevice(const std::vector<VkPhysicalDevice> availablePhysicalDevices);
    bool checkDeviceExtensionsSupport(VkPhysicalDevice physicalDevice);
    void setupLogicalDevice(VulkanSurface& surface);

    VkInstance* instance;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;

    const std::vector<const char*> deviceExtensions =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    uint32_t graphicsQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex;
    
    VkQueue graphicsQueue;
    VkQueue presentQueue;
};

#endif // VULKANPHYSICALDEVICE_H