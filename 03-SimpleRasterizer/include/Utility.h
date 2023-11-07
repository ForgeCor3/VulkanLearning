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

    VkImageView createImageView(VkDevice* logicalDevice, VkImage* image, VkFormat format);

    std::vector<char> readFile(const std::string& fileName);
    void createImage(VkDevice* logicalDevice, VkPhysicalDevice* physicalDevice,  uint32_t width, uint32_t height, VkFormat format,
        VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* textureImage, VkDeviceMemory* textureImageMemory);
    VkShaderModule createShaderModule(const std::vector<char> shaderCode, VkDevice* logicalDevice);

    VkCommandBuffer beginSingleTimeCommand(VkDevice* logicalDevice, VkCommandPool* commandPool);
    void endSingleTimeCommand(VkDevice* logicalDevice, VkCommandPool* commandPool, VkCommandBuffer* commandBuffer, VkQueue* graphicsQueue);

    void createBuffer(VkDevice* logicalDevice, VkPhysicalDevice* physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    uint32_t findMemoryType(VkPhysicalDevice* physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void copyBuffer(VkDevice* logicalDevice, VkBuffer* srcBuffer, VkBuffer* dstBuffer, VkDeviceSize size, VkCommandPool* commandPool, VkQueue* queue);
    void copyBufferToImage(VkDevice* logicalDevice, VkCommandPool* commandPool, VkQueue* graphicsQueue, VkBuffer* buffer, VkImage* image, uint32_t width, uint32_t height);

    void transitionImageLayout(VkDevice* logicalDevice, VkCommandPool* commandPool, VkImage* image, VkQueue* graphicsQueue, VkFormat format, 
        VkImageLayout oldLayout, VkImageLayout newLayout);

} //namespace utility

#endif //UTILITY_H