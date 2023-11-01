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
    bool checkValidationLayerSupport(const std::vector<const char*> validationLayers);
    std::vector<const char*> getRequiredExtensions(const bool enableValidationLayers);

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    bool isPhysicalDeviceSuitable(VkPhysicalDevice& physicalDevice, VkSurfaceKHR* surface, const std::vector<const char*>& deviceExtensions);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice* physicalDevice, VkSurfaceKHR* surface);
    bool checkDeviceExtensionSupport(VkPhysicalDevice* physicalDevice, const std::vector<const char*> deviceExtensions);

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice* physicalDevice, VkSurfaceKHR* surface);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

    std::vector<char> readFile(const std::string& fileName);
    VkShaderModule createShaderModule(const std::vector<char> shaderCode, VkDevice* logicalDevice);

    void createBuffer(VkDevice* logicalDevice, VkPhysicalDevice* physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    uint32_t findMemoryType(VkPhysicalDevice* physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void copyBuffer(VkDevice* logicalDevice_, VkBuffer* srcBuffer, VkBuffer* dstBuffer, VkDeviceSize size, VkCommandPool* commandPool_, VkQueue* queue_);


    void recreateSwapChain();

} //namespace utility

#endif //UTILITY_H