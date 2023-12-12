#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <vector>

#include "GlobUtils.h"
#include "Window.h"
#include "Vulkan/VulkanInstance.h"
#include "Vulkan/VulkanDebugUtilsMessenger.h"
#include "Vulkan/VulkanSurface.h"
#include "Vulkan/VulkanDevice.h"
#include "Vulkan/VulkanSwapChain.h"
#include "Vulkan/VulkanGraphicsPipeline.h"
#include "Vulkan/VulkanCommandPool.h"
#include "Vulkan/VulkanCommandBuffer.h"
#include "Vulkan/VulkanSemaphore.h"
#include "Vulkan/VulkanFence.h"

class Application final
{
public:
    DISABLE_COPY_AND_MOVE(Application)

    Application() = delete;
    Application(const WindowConfig& windowConfig, bool validationLayersEnabled);
    ~Application();

    void run();
    void mainloop();
    void terminate();

private:
    void drawFrame();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void createSwapchain();
    void destroySwapchain();
    void recreateSwapchain();

    std::vector<const char*> validationLayers;

    std::unique_ptr<Window> window;

    std::unique_ptr<VulkanInstance> instance;
    std::unique_ptr<VulkanDebugUtilsMessenger> debugUtilsMessenger;

    std::unique_ptr<VulkanDevice> device;
    std::unique_ptr<VulkanSurface> surface;

    std::unique_ptr<VulkanSwapChain> swapChain;
    std::unique_ptr<VulkanGraphicsPipeline> graphicsPipeline;

    std::unique_ptr<VulkanCommandPool> commandPool;
    std::vector<std::unique_ptr<VulkanCommandBuffer>> commandBuffers;

    std::vector<std::unique_ptr<VulkanSemaphore>> imageAvailableSemaphores;
    std::vector<std::unique_ptr<VulkanSemaphore>> renderFinishedSemaphores;
    std::vector<std::unique_ptr<VulkanFence>> inFlightFences;

    uint32_t currentFrame;
};

#endif // APPLICATION_H