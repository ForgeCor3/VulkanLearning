#include "Vulkan/VulkanFramebuffer.h"

VulkanFramebuffer::VulkanFramebuffer(VulkanRenderPass& renderPass, const VkImageView& imageView)
	: device(renderPass.getSwapChain().getDevice())
{
	VkFramebufferCreateInfo framebufferCreateInfo{};
	framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferCreateInfo.renderPass = renderPass.getRenderPass();
	framebufferCreateInfo.attachmentCount = 1;
	framebufferCreateInfo.pAttachments = &imageView;
	framebufferCreateInfo.width = renderPass.getSwapChain().getExtent().width;
	framebufferCreateInfo.height = renderPass.getSwapChain().getExtent().height;
	framebufferCreateInfo.layers = 1;

	if (vkCreateFramebuffer(renderPass.getSwapChain().getDevice(), &framebufferCreateInfo, nullptr, &framebuffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to create framebuffer.");
}

VulkanFramebuffer::~VulkanFramebuffer()
{
	vkDestroyFramebuffer(device, framebuffer, nullptr);
}

VkFramebuffer& VulkanFramebuffer::getFramebuffer()
{
	return framebuffer;
}