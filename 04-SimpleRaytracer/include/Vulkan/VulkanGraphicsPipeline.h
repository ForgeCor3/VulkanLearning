#ifndef VULKANGRAPHICSPIPELINE_H
#define VULKANGRAPHICSPIPELINE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "GlobUtils.h"
#include "Vulkan/VulkanDevice.h"
#include "Vulkan/VulkanShaderModule.h"
#include "Vulkan/VulkanPipelineLayout.h"
#include "Vulkan/VulkanRenderPass.h"

class VulkanGraphicsPipeline final
{
public:
    DISABLE_COPY_AND_MOVE(VulkanGraphicsPipeline)

    VulkanGraphicsPipeline() = delete;
    VulkanGraphicsPipeline(VulkanSwapChain& swapChain);
    ~VulkanGraphicsPipeline();

private:
    VkDevice& device;

    std::unique_ptr<VulkanPipelineLayout> pipelineLayout;
    std::unique_ptr<VulkanRenderPass> renderPass;

    VkPipeline graphicsPipeline;
};

#endif // VULKANGRAPHICSPIPELINE_H