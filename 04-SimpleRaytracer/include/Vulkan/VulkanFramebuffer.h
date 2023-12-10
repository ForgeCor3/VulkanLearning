#ifndef VULKANFRAMEBUFFER_H
#define VULKANFRAMEBUFFER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "GlobUtils.h"
#include "VulkanRenderPass.h"
#include "VulkanImageView.h"

class VulkanFramebuffer final
{
public:
    VulkanFramebuffer() = delete;
    VulkanFramebuffer(VulkanRenderPass& renderPass, const VkImageView& imageView);
    ~VulkanFramebuffer();

private:
    VkDevice& device;
    VkFramebuffer framebuffer;
};

#endif // VULKANFRAMEBUFFER_H