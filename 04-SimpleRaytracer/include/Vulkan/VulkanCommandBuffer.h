#ifndef VULKANCOMMANDBUFFER_H
#define VULKANCOMMANDBUFFER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <memory>

#include "GlobUtils.h"
#include "VulkanCommandPool.h"

class VulkanCommandBuffer final
{
public:
    DISABLE_COPY_AND_MOVE(VulkanCommandBuffer)

    VulkanCommandBuffer() = delete;
    VulkanCommandBuffer(VkDevice& device, VkCommandPool& commandPool);
    ~VulkanCommandBuffer();

    VkCommandBuffer& getCommandBuffer();

    void commandBufferBegin();

private:
    VkCommandBuffer commandBuffer;
};

#endif // VULKANCOMMANDBUFFER_H