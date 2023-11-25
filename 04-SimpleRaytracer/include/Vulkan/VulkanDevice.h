#ifndef VULKANDEVICE_H
#define VULKANDEVICE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <stdexcept>
#include <map>
#include <optional>

#include "GlobUtils.h"
#include "Vulkan/VulkanUtils.h"

class VulkanDevice final
{
public:
    DISABLE_COPY_AND_MOVE(VulkanDevice)

    VulkanDevice() = delete;
    VulkanDevice(VkInstance& instance);
    ~VulkanDevice();

private:
    std::optional<uint32_t> findQueue(VkQueueFlags queueFlags, const VkPhysicalDevice physicalDevice);

    void selectPhysicalDevice();
    VkPhysicalDevice findSuitablePhysicalDevice(const std::vector<VkPhysicalDevice> availablePhysicalDevices);
    void setupLogicalDevice();

    VkInstance* instance;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;

    uint32_t graphicsQueueFamilyIndex;
    
    VkQueue graphicsQueue;
};

#endif // VULKANPHYSICALDEVICE_H