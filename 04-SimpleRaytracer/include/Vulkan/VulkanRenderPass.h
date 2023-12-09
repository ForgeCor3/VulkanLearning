#ifndef VULKANRENDERPASS_H
#define VULKANRENDERPASS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "GlobUtils.h"
#include "Vulkan/VulkanSwapChain.h"

class VulkanRenderPass final
{
public:
    DISABLE_COPY_AND_MOVE(VulkanRenderPass)

    VulkanRenderPass() = delete;
    VulkanRenderPass(VulkanSwapChain& swapChain);
    ~VulkanRenderPass();

    VkRenderPass getRenderPass();

private:
    VkDevice& device;

    VkRenderPass renderPass;
};

#endif //VULKANRENDERPASS_H