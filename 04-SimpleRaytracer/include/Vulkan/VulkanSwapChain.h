#ifndef VULKANSWAPCHAIN_H
#define VULKANSWAPCHAIN_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

#include "GlobUtils.h"

class VulkanSwapChain final
{    
public:
    DISABLE_COPY_AND_MOVE(VulkanSwapChain)

    VulkanSwapChain();
    ~VulkanSwapChain();

private:
    struct SupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };
    
};

#endif // VULKANSWAPCHAIN_H