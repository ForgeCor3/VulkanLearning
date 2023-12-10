#ifndef VULKANCOMMANDPOOL_H
#define VULKANCOMMANDPOOL_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "GlobUtils.h"

class VulkanCommandPool final
{
public:
    DISABLE_COPY_AND_MOVE(VulkanCommandPool)

    VulkanCommandPool() = delete;
    VulkanCommandPool(const VkDevice& device, const uint32_t queueFamilyIndex);
    ~VulkanCommandPool();

private:
    const VkDevice& device;
    VkCommandPool commandPool;
};

#endif // VULKANCOMMANDPOOL_H