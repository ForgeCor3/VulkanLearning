#include "Vulkan/VulkanPipelineLayout.h"

VulkanPipelineLayout::VulkanPipelineLayout(VkDevice& device)
	: device(device)
{
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	if (vkCreatePipelineLayout(this->device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		throw std::runtime_error("Failed to create pipeline layout");
}

VulkanPipelineLayout::~VulkanPipelineLayout()
{
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
}

VkPipelineLayout VulkanPipelineLayout::getPipelineLayout()
{
	return pipelineLayout;
}