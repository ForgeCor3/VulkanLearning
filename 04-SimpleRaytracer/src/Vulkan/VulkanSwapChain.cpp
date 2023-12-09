#include "Vulkan/VulkanSwapChain.h"

VulkanSwapChain::VulkanSwapChain(VulkanDevice& device) : device(device.getDevice())
{
    VulkanSwapChain::SupportDetails supportDetails = querySwapChainSupport(device);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(supportDetails.surfaceFormats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(supportDetails.presentModes);

    imageFormat = surfaceFormat.format;
    extent = chooseSwapExtent(supportDetails.capabilities, device);
    imageCount = chooseImageCount(supportDetails.capabilities);

    VkSwapchainCreateInfoKHR swapchainCreateInfo {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = device.getSurface();
    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = imageFormat;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = extent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if(device.getGraphicsQueueFamilyIndex() != device.getPresentQueueFamilyIndex())
    {
        uint32_t queueFamilyIndices[] = { device.getGraphicsQueueFamilyIndex(), device.getPresentQueueFamilyIndex() };

        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    }
    swapchainCreateInfo.preTransform = supportDetails.capabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if(vkCreateSwapchainKHR(this->device, &swapchainCreateInfo, nullptr, &swapChain) != VK_SUCCESS)
        throw std::runtime_error("Failed to create swap chain.");

    EnumerateVector(vkGetSwapchainImagesKHR, this->device, swapChain, swapChainImages);

    for(const auto& image : swapChainImages)
        swapChainImageViews.push_back(std::make_unique<VulkanImageView>(this->device, image, imageFormat));
}

VulkanSwapChain::~VulkanSwapChain()
{
    for(auto& imageView : swapChainImageViews)
        imageView.reset();

    vkDestroySwapchainKHR(device, swapChain, nullptr);
}

VkDevice& VulkanSwapChain::getDevice() { return device; }

VkExtent2D VulkanSwapChain::getExtent() { return extent; }

VkFormat VulkanSwapChain::getFormat() { return imageFormat; }

VulkanSwapChain::SupportDetails VulkanSwapChain::querySwapChainSupport(VulkanDevice& device)
{
    VulkanSwapChain::SupportDetails supportDetails;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.getPhysicalDevice(), device.getSurface(), &supportDetails.capabilities);
    EnumerateVector(vkGetPhysicalDeviceSurfaceFormatsKHR, device.getPhysicalDevice(), device.getSurface(), supportDetails.surfaceFormats);
    EnumerateVector(vkGetPhysicalDeviceSurfacePresentModesKHR, device.getPhysicalDevice(), device.getSurface(), supportDetails.presentModes);

    if(supportDetails.presentModes.empty() || supportDetails.presentModes.empty())
        throw std::runtime_error("Physical device doesn't support swap chain.");
    
    return supportDetails;
}

VkSurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for(const auto& format : availableFormats)
        if(format.format == VK_FORMAT_B8G8R8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return format;

    return availableFormats[0];
}

VkPresentModeKHR VulkanSwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for(const auto& presentMode : availablePresentModes)
        if(presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return presentMode;
    
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, VulkanDevice& device)
{
    if(surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return surfaceCapabilities.currentExtent;
    else
    {
        int width;
        int height;
        glfwGetFramebufferSize(device.getWindow(), &width, &height);

        VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

        actualExtent.width = std::clamp(
            actualExtent.width,
            surfaceCapabilities.minImageExtent.width,
            surfaceCapabilities.maxImageExtent.width
        );
        actualExtent.height = std::clamp(
            actualExtent.height,
            surfaceCapabilities.maxImageExtent.height,
            surfaceCapabilities.maxImageExtent.height  
        );

        return actualExtent;
    }
}

uint32_t VulkanSwapChain::chooseImageCount(const VkSurfaceCapabilitiesKHR& capabilities)
{
    uint32_t imageCount;
    imageCount = capabilities.minImageCount + 1;

    if(capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
        imageCount = capabilities.maxImageCount;

    return imageCount;
}