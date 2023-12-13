#ifndef VULKANIMAGEVIEW_H
#define VULKANIMAGEVIEW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "GlobUtils.h"

class VulkanImageView final
{
public:
	DISABLE_COPY_AND_MOVE(VulkanImageView)

	VulkanImageView() = delete;
	VulkanImageView(const VkDevice& device, const VkImage image, const VkFormat imageFormat);
	~VulkanImageView();

	VkImageView& getImageView();

private:
	const VkDevice& device;
	VkImageView		imageView;
};

#endif // VULKANIMAGEVIEW_H