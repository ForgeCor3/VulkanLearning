#ifndef VULKANGRAPHICSPIPELINE_H
#define VULKANGRAPHICSPIPELINE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "GlobUtils.h"
#include "Vulkan/VulkanDevice.h"
#include "Vulkan/VulkanShaderModule.h"
#include "Vulkan/VulkanPipelineLayout.h"

class VulkanGraphicsPipeline final
{
public:
    DISABLE_COPY_AND_MOVE(VulkanGraphicsPipeline)

    VulkanGraphicsPipeline() = delete;
    VulkanGraphicsPipeline(VulkanSwapChain& swapChain);
    ~VulkanGraphicsPipeline();

private:
    VkDevice& device;

    //VkPipelineLayout pipelineLayout;
    std::unique_ptr<VulkanPipelineLayout> pipelineLayout;
};

#endif // VULKANGRAPHICSPIPELINE_H