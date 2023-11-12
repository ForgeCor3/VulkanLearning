#ifndef VULKANINITIALIZATION_H
#define VULKANINITIALIZATION_H

#include <map>

#include <stb_image.h>
#include <tiny_obj_loader.h>

#include "Utility.h"
#include "Extensions.h"

#include "ModelOBJ.h"
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

    void pickPhysicalDevice(VkInstance* instance, VkPhysicalDevice* physicalDevice, VkSurfaceKHR* surface, VkSampleCountFlagBits& msaaSamples);
    void createLogicalDevice(VkPhysicalDevice* physicalDevice, VkSurfaceKHR* surface, VkDevice* logicalDevice, VkQueue* graphicsQueue, VkQueue* presentQueue);

    void createSwapChain(VkDevice* logicalDevice, VkPhysicalDevice* physicalDevice, VkSurfaceKHR* surface, GLFWwindow* window, VkSwapchainKHR* swapChain,
        std::vector<VkImage>& swapChainImages, VkFormat& swapChainImageFormat, VkExtent2D& swapChainExtent);
    void createSwapChainImageViews(VkDevice* logicalDevice, std::vector<VkImageView>& swapChainImageViews, std::vector<VkImage> swapChainImages, VkFormat* swapChainImageFormat);

    void createRenderPass(VkDevice* logicalDevice, VkPhysicalDevice* physicalDevice, VkRenderPass* renderPass, VkFormat* swapChainImageFormat, VkSampleCountFlagBits msaaSamples);
    void createDescriptorSetLayout(VkDevice* logicalDevice, VkDescriptorSetLayout* descriptorSetLayout);
    void createGraphicsPipeline(VkDevice* logicalDevice, VkPipeline* graphicsPipeline, VkExtent2D* swapChainExtent, VkPipelineLayout* pipelineLayout,
        VkRenderPass* renderPass, VkDescriptorSetLayout* descriptorSetLayout, VkSampleCountFlagBits msaaSamples);

    void createFramebuffers(VkDevice* logicalDevice, std::vector<VkFramebuffer>& swapChainFramebuffers, std::vector<VkImageView> swapChainImageViews,
        VkImageView* depthImageView, VkImageView* colorImageView, VkRenderPass* renderPass, VkExtent2D* swapChainExtent);
    void createCommandPool(VkDevice* logicalDevice, VkCommandPool* commandPool, VkPhysicalDevice* physicalDevice, VkSurfaceKHR* surface);
    void createColorResources(VkDevice* logicalDevice, VkPhysicalDevice* physicalDevice, VkFormat* swapChainImageFormat, VkExtent2D* swapChainExtent, VkSampleCountFlagBits msaaSamples,
        VkImage* colorImage, VkDeviceMemory* colorImageMemory, VkImageView* colorImageView);
    void createDepthResources(VkDevice* logicalDevice, VkPhysicalDevice* physicalDevice, VkExtent2D swapChainExtent,
        VkImage* depthImage, VkDeviceMemory* depthImageMemory, VkImageView& depthImageView, VkSampleCountFlagBits msaaSamples);
    void createTextureImage(VkDevice* logicalDevice, VkPhysicalDevice* physicalDevice, VkImage* textureImage, VkDeviceMemory* textureImageMemory, VkCommandPool* commandPool,
        VkQueue* graphicsQueue, uint32_t& mipLevels);
    void createTextureImageView(VkDevice* logicalDevice, VkImageView& textureImageView, VkImage* textureImage, uint32_t mipLevels);
    void createTextureSampler(VkDevice* logicalDevice, VkPhysicalDevice* physicalDevice, VkSampler* textureSampler, uint32_t mipLevels);

    void loadModel(ModelOBJ& model, std::string modelPath);

    void createVertexBuffer(VkDevice* logicalDevice, VkBuffer* vertexBuffer, VkDeviceMemory* vertexBufferMemory,
        VkCommandPool* commandPool, VkQueue* queue, VkPhysicalDevice* physicalDevice, ModelOBJ model);
    void createIndexBuffer(VkDevice* logicalDevice, VkBuffer* indexBuffer, VkDeviceMemory* indexBufferMemory,
        VkCommandPool* commandPool, VkQueue* queue, VkPhysicalDevice* physicalDevice, ModelOBJ model);
    void createUniformBuffers(VkDevice* logicalDevice, VkPhysicalDevice* physicalDevice, std::vector<VkBuffer>& uniformBuffers, std::vector<VkDeviceMemory>& uniformBuffersMemory,
        std::vector<void*>& unifromBuffersMapped, const int MAX_FRAMES_IN_FLIGHT);
    void createDescriptorPool(VkDevice* logicalDevice, VkDescriptorPool* descriptorPool, const int MAX_FRAMES_IN_FLIGHT);
    void createDescriptorSets(VkDevice* logicalDevice, std::vector<VkDescriptorSet>& descriptorSets, VkDescriptorSetLayout* descriptorSetLayout, VkDescriptorPool* descriptorPool,
        std::vector<VkBuffer> uniformBuffers, VkImageView* textureImageView, VkSampler* textureSampler, const int MAX_FRAMES_IN_FLIGHT);
    void createCommandBuffers(VkDevice* logicalDevice, const int MAX_FRAMES_IN_FLIGHT, std::vector<VkCommandBuffer>& commandBuffers, VkCommandPool* commandPool);

    void createSyncObjects(VkDevice* logicalDevice, std::vector<VkSemaphore>& imageAvailableSemaphores, std::vector<VkSemaphore>& renderFinishedSemaphores, std::vector<VkFence>& inFlightFences,
        const int MAX_FRAMES_IN_FLIGHT);

} //namespace vulkanInitialization

#endif //VULKANINITIALIZATION_H