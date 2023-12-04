#ifndef VULKANSWAPCHAIN_H
#define VULKANSWAPCHAIN_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <cstdint>
#include <limits>
#include <algorithm>

#include "GlobUtils.h"
#include "Vulkan/VulkanUtils.h"
#include "Vulkan/VulkanDevice.h"
#include "Vulkan/VulkanImageView.h"

class VulkanSwapChain final
{    
public:
    DISABLE_COPY_AND_MOVE(VulkanSwapChain)

    VulkanSwapChain() = delete;
    VulkanSwapChain(VulkanDevice& device);
    ~VulkanSwapChain();

private:
    struct SupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> surfaceFormats;
        std::vector<VkPresentModeKHR> presentModes;
    };
    
    VulkanSwapChain::SupportDetails querySwapChainSupport(VulkanDevice& device);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, VulkanDevice& device);
    uint32_t chooseImageCount(const VkSurfaceCapabilitiesKHR& capabilities);

    VkDevice& device;

    VkSwapchainKHR swapChain;

    std::vector<VkImage> swapChainImages;
    std::vector<std::unique_ptr<VulkanImageView>> swapChainImageViews;
    VkFormat imageFormat;
    VkExtent2D extent;
    uint32_t imageCount;
};

#endif // VULKANSWAPCHAIN_H