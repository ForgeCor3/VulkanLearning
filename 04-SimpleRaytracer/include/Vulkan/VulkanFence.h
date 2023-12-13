#ifndef VULKANFENCE_H
#define VULKANFENCE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "GlobUtils.h"

class VulkanFence final
{
public:
	DISABLE_COPY_AND_MOVE(VulkanFence)

	VulkanFence() = delete;
	VulkanFence(const VkDevice& device, const bool isSignaled);
	~VulkanFence();

	VkFence& getFence();

private:
	const VkDevice& device;
	VkFence			fence;
};

#endif // VULKANFENCE_H