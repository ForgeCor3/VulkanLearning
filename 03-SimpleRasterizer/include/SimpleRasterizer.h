#ifndef SIMPLERASTERIZER_H
#define SIMPLERASTERIZER_H

#include "Extensions.h"
#include "VulkanInitialization.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
const int MAX_FRAMES_IN_FLIGHT = 2;

class SimpleRasterizer
{
public:
	void run();

private:

	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanUp();
	void cleanUpSwapChain();

	void recordCommandBuffer(VkCommandBuffer _commandBuffer, uint32_t imageIndex);
	void recreateSwapChain();
	void drawFrame();

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto app = reinterpret_cast<SimpleRasterizer*>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}

	GLFWwindow* window;

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;

	VkSurfaceKHR surface;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice logicalDevice;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;

	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkCommandPool commandPool;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	std::vector<VkCommandBuffer> commandBuffers;
	
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	uint32_t currentFrame = 0;
	bool framebufferResized = false;
};

#endif //TRIANGLEAPP_H