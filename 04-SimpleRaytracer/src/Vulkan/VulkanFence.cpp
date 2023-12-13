#include "Vulkan/VulkanFence.h"

VulkanFence::VulkanFence(const VkDevice& device, const bool isSignaled)
	: device(device)
{
	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	if (isSignaled)
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (vkCreateFence(device, &fenceCreateInfo, nullptr, &fence) != VK_SUCCESS)
		throw std::runtime_error("Failed to create fence.");
}

VulkanFence::~VulkanFence()
{
	vkDestroyFence(device, fence, nullptr);
}

VkFence& VulkanFence::getFence()
{
	return fence;
}