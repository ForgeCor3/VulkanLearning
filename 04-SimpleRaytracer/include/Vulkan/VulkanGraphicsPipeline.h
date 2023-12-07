#ifndef VULKANGRAPHICSPIPELINE_H
#define VULKANGRAPHICSPIPELINE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "GlobUtils.h"
#include "Vulkan/VulkanDevice.h"
#include "Vulkan/VulkanShaderModule.h"

class VulkanGraphicsPipeline final
{
public:
    DISABLE_COPY_AND_MOVE(VulkanGraphicsPipeline)

    VulkanGraphicsPipeline() = delete;
    VulkanGraphicsPipeline(VulkanDevice& device);
    ~VulkanGraphicsPipeline();

private:
    VkDevice& device;
};

#endif // VULKANGRAPHICSPIPELINE_H