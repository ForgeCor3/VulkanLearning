#ifndef VULKANINITIALIZATION_H
#define VULKANINITIALIZATION_H

#include "Utility.h"
#include "Extensions.h"

#include "VerticesData.h"
#include "UniformBufferObject.h"

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

    void createInstance(VkInstance* instance);
    void setupDebugMessenger(VkInstance* instance, VkDebugUtilsMessengerEXT* debugMessenger);

    void createSurface(VkInstance* instance, GLFWwindow* window, VkSurfaceKHR* surface);

    void pickPhysicalDevice(VkInstance* instance, VkPhysicalDevice* physicalDevice, VkSurfaceKHR* surface);
    void createLogicalDevice(VkPhysicalDevice* physicalDevice, VkSurfaceKHR* surface, VkDevice* logicalDevice, VkQueue* graphicsQueue, VkQueue* presentQueue);

    void createSwapChain(VkDevice* logicalDevice, VkPhysicalDevice* physicalDevice, VkSurfaceKHR* surface, GLFWwindow* window, VkSwapchainKHR* swapChain,
        std::vector<VkImage>& swapChainImages, VkFormat& swapChainImageFormat, VkExtent2D& swapChainExtent);
    void createSwapChainImageViews(VkDevice* logicalDevice, std::vector<VkImageView>& swapChainImageViews, std::vector<VkImage> swapChainImages, VkFormat* swapChainImageFormat);

    void createRenderPass(VkDevice* logicalDevice, VkRenderPass* renderPass, VkFormat* swapChainImageFormat);
    void createDescriptorSetLayout(VkDevice* logicalDevice, VkDescriptorSetLayout* descriptorSetLayout);
    void createGraphicsPipeline(VkDevice* logicalDevice, VkPipeline* graphicsPipeline, VkExtent2D* swapChainExtent, VkPipelineLayout* pipelineLayout,
        VkRenderPass* renderPass, VkDescriptorSetLayout* descriptorSetLayout);

    void createFramebuffers(VkDevice* logicalDevice, std::vector<VkFramebuffer>& swapChainFramebuffers, std::vector<VkImageView> swapChainImageViews, VkRenderPass* renderPass,
        VkExtent2D* swapChainExtent);
    void createCommandPool(VkDevice* logicalDevice, VkCommandPool* commandPool, VkPhysicalDevice* physicalDevice, VkSurfaceKHR* surface);
    void createVertexBuffer(VkDevice* logicalDevice, VkBuffer* vertexBuffer, VkDeviceMemory* vertexBufferMemory, VkCommandPool* commandPool, VkQueue* queue, VkPhysicalDevice* physicalDevice);
    void createIndexBuffer(VkDevice* logicalDevice, VkBuffer* indexBuffer, VkDeviceMemory* indexBufferMemory, VkCommandPool* commandPool, VkQueue* queue, VkPhysicalDevice* physicalDevice);
    void createUniformBuffers();
    void createCommandBuffers(VkDevice* logicalDevice, const int MAX_FRAMES_IN_FLIGHT, std::vector<VkCommandBuffer>& commandBuffers, VkCommandPool* commandPool);
    void createSyncObjects(VkDevice* logicalDevice, std::vector<VkSemaphore>& imageAvailableSemaphores, std::vector<VkSemaphore>& renderFinishedSemaphores, std::vector<VkFence>& inFlightFences,
        const int MAX_FRAMES_IN_FLIGHT);

} //namespace vulkanInitialization

#endif //VULKANINITIALIZATION_H