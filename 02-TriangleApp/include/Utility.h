#ifndef UTILITY_H
#define UTILITY_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"

#include <vector>
#include <set>
#include <iostream>
#include <algorithm>
#include <fstream>

namespace utility
{
    bool checkValidationLayerSupport(const std::vector<const char*>& _validationLayers);
    std::vector<const char*> getRequiredExtensions(const bool _enableValidationLayers);
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);
    bool isPhysicalDeviceSuitable(VkPhysicalDevice& _physicalDevice, VkSurfaceKHR* _surface, const std::vector<const char*>& _deviceExtensions);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice* _physicalDevice, VkSurfaceKHR* _surface);
    bool checkDeviceExtensionSupport(VkPhysicalDevice* _physicalDevice, const std::vector<const char*>& deviceExtensions);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice* _physicalDevice, VkSurfaceKHR* _surface);

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* _window);

    std::vector<char> readFile(const std::string& fileName);
    VkShaderModule createShaderModule(const std::vector<char>& shaderCode, VkDevice* _logicalDevice);

    void createBuffer(VkDevice* logicalDevice_, VkDeviceSize size, VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory, VkPhysicalDevice* physicalDevice_);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice* physicalDevice_);
    void copyBuffer(VkDevice* logicalDevice_, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool* commandPool_,
        VkQueue* queue_);


    void recreateSwapChain();
} //namespace utility

#endif //UTILITY_H