#ifndef VULKANPHYSICALDEVICE_H
#define VULKANPHYSICALDEVICE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <stdexcept>

#include "GlobUtils.h"
#include "Vulkan/VulkanUtils.h"

class VulkanPhysicalDevice
{
private:
    DISABLE_COPY_AND_MOVE(VulkanPhysicalDevice)

    void selectPhysicalDevice();

    VkInstance* instance;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

public:
    VulkanPhysicalDevice() = delete;
    VulkanPhysicalDevice(VkInstance& instance);

    ~VulkanPhysicalDevice() { };
};

#endif // VULKANPHYSICALDEVICE_H