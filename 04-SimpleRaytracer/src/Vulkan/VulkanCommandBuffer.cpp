#include "Vulkan/VulkanCommandBuffer.h"

VulkanCommandBuffer::VulkanCommandBuffer(VkDevice& device, VkCommandPool& commandPool)
{
	VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool = commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate command buffer.");
}

VulkanCommandBuffer::~VulkanCommandBuffer() {}

VkCommandBuffer& VulkanCommandBuffer::getCommandBuffer()
{
	return commandBuffer;
}

void VulkanCommandBuffer::commandBufferBegin()
{
	VkCommandBufferBeginInfo commandBufferBeginInfo{};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) != VK_SUCCESS)
		throw std::runtime_error("Failed to begin command buffer.");
}