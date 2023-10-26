#ifndef VULKANINITIALIZATION_H
#define VULKANINITIALIZATION_H

#include "Utility.h"
#include "Extensions.h"
#include "Vertex.h"

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

namespace vulkanInitialization
{
    const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    void createInstance(VkInstance& _instance);
    void setupDebugMessenger(VkInstance& _instance, VkDebugUtilsMessengerEXT& _debugMessenger);
    void createSurface(VkInstance& _instance, GLFWwindow& _window, VkSurfaceKHR& _surface);
    void pickPhysicalDevice(VkInstance& _instance, VkPhysicalDevice& _physicalDevice,
        VkSurfaceKHR* _surface, const std::vector<const char*>& _deviceExtensions);
    void createLogicalDevice(VkPhysicalDevice* _physicalDevice, VkSurfaceKHR* _surface,
        VkDevice& _logicalDevice, VkQueue& _graphicsQueue, VkQueue& _presentQueue);
    void createSwapChain(VkDevice* _logicalDevice, VkPhysicalDevice* _physicalDevice, VkSurfaceKHR* _surface,
        GLFWwindow* _window, VkSwapchainKHR& _swapChain, std::vector<VkImage>& _swapChainImages,
        VkFormat& swapChainImageFormat, VkExtent2D& swapChainExtent);
    void createSwapChainImageViews(std::vector<VkImageView>& _swapChainImageViews, std::vector<VkImage> _swapChainImages,
    VkFormat* _swapChainImageFormat, VkDevice* _logicalDevice);
    void createRenderPass(VkDevice* _logicalDevice, VkFormat* _swapChainImageFormat, VkRenderPass& _renderPass);
    void createGraphicsPipeline(VkDevice* _logicalDevice, VkExtent2D* _swapChainExtent, VkPipelineLayout& _pipelineLayout,
        VkRenderPass* _renderPass, VkPipeline& _graphicsPipeline);
} //namespace vulkanInitialization

#endif //VULKANINITIALIZATION_H