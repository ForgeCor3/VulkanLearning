#ifndef VULKANGRAPHICSPIPELINE_H
#define VULKANGRAPHICSPIPELINE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>

#include "GlobUtils.h"
#include "Vulkan/VulkanDevice.h"
#include "Vulkan/VulkanShaderModule.h"
#include "Vulkan/VulkanPipelineLayout.h"
#include "Vulkan/VulkanRenderPass.h"
#include "Vulkan/VulkanFramebuffer.h"

class VulkanGraphicsPipeline final
{
public:
	DISABLE_COPY_AND_MOVE(VulkanGraphicsPipeline)

	VulkanGraphicsPipeline() = delete;
	VulkanGraphicsPipeline(VulkanSwapChain& swapChain);
	~VulkanGraphicsPipeline();

	VkPipeline&										 getPipeline();
	VulkanRenderPass&								 getVulkanRenderPass();
	std::vector<std::unique_ptr<VulkanFramebuffer>>& getVulkanFramebuffers();

private:
	VkDevice&  device;
	VkPipeline graphicsPipeline;

	std::unique_ptr<VulkanPipelineLayout>			pipelineLayout;
	std::unique_ptr<VulkanRenderPass>				renderPass;
	std::vector<std::unique_ptr<VulkanFramebuffer>> framebuffers;
};

#endif // VULKANGRAPHICSPIPELINE_H