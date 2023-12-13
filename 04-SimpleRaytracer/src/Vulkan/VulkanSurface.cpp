#include "Vulkan/VulkanSurface.h"

VulkanSurface::VulkanSurface(VulkanInstance* instance)
{
	this->instance = instance->getInstance();

	if (glfwCreateWindowSurface(instance->getInstance(), instance->getWindow(), nullptr, &surface) != VK_SUCCESS)
		throw std::runtime_error("Failed to create surface.");
}

VulkanSurface::~VulkanSurface()
{
	vkDestroySurfaceKHR(instance, surface, nullptr);
}

const VkSurfaceKHR VulkanSurface::getSurface()
{
	return surface;
}