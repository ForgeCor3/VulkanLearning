#ifndef VULKANPIPELINELAYOUT_H
#define VULKANPIPELINELAYOUT_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "GlobUtils.h"

class VulkanPipelineLayout final
{
public:
    DISABLE_COPY_AND_MOVE(VulkanPipelineLayout)

    VulkanPipelineLayout() = delete;
    VulkanPipelineLayout(VkDevice& device);
    ~VulkanPipelineLayout();

    VkPipelineLayout getPipelineLayout();

private:
    VkDevice& device;
    VkPipelineLayout pipelineLayout;
};

#endif //VULKANPIPELINELAYOUT_H