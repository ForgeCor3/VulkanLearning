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
    DISABLE_COPY_AND_MOVE(VulkanFramebuffer)

    VulkanFramebuffer() = delete;
    VulkanFramebuffer(VulkanRenderPass& renderPass, const VkImageView& imageView);
    ~VulkanFramebuffer();

    VkFramebuffer& getFramebuffer();

private:
    VkDevice& device;
    VkFramebuffer framebuffer;
};

#endif // VULKANFRAMEBUFFER_H