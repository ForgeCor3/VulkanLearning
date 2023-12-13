#ifndef VULKANSEMAPHORE_H
#define VULKANSEMAPHORE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "GlobUtils.h"

class VulkanSemaphore final
{
public:
	DISABLE_COPY_AND_MOVE(VulkanSemaphore)

	VulkanSemaphore() = delete;
	VulkanSemaphore(const VkDevice& device);
	~VulkanSemaphore();

	VkSemaphore& getSemaphore();

private:
	const VkDevice& device;
	VkSemaphore		semaphore;
};

#endif // VULKANSEMAPHORE_H