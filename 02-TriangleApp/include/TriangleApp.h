#ifndef TRIANGLEAPP_H
#define TRIANGLEAPP_H

#include "Extensions.h"
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <optional>
#include <set>
#include <array>
#include <limits>
#include <algorithm>
#include <fstream>
#include <glm/glm.hpp>

#include "VulkanInitialization.h"
#include "TriangleData.h" // TO COMMENT

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = { // TO COMMENT
	"VK_LAYER_KHRONOS_validation" 
};

const std::vector<const char*> deviceExtensions = { // TO COMMENT
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// #ifdef NDEBUG
// 	const bool enableValidationLayers = false;
// #else
// 	const bool enableValidationLayers = true;
// #endif

class TriangleApp
{
public:
	void run();

private:
	struct QueueFamilyIndices // TO COMMENT
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		bool isComplete() {return graphicsFamily.has_value() && presentFamily.has_value();}
	};

	struct SwapChainSupportDetails // TO COMMENT
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct Vertex
	{
		glm::vec2 position;
		glm::vec3 color;

		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescription()
		{
			std::array<VkVertexInputAttributeDescription, 2> attributeDescription;
			attributeDescription[0].binding = 0;
			attributeDescription[0].location = 0;
			attributeDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescription[0].offset = offsetof(Vertex, position);

			attributeDescription[1].binding = 0;
			attributeDescription[1].location = 1;
			attributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescription[1].offset = offsetof(Vertex, color);

			return attributeDescription;
		}
	};

	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanUp();
	void cleanUpSwapChain();

	//void createInstance();
	//void setupDebugMessenger();
	//void createSurface();
	//void pickPhysicalDevice();
	//void createLogicalDevice();
	// void createSwapChain();
	//void createSwapChainImageViews();
	//void createRenderPass();
	//void createGraphicsPipeline();
	//VkShaderModule createShaderModule(const std::vector<char>& shaderCode);
	//void createFramebuffers();
	void createCommandPool();
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void createVertexBuffer();
	void createCommandBuffers();
	void createSyncObjects();

	//void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	//bool checkValidationLayerSupport();
	//std::vector<const char*> getRequiredExtensions();
	//bool isPhysicalDeviceSuitable(VkPhysicalDevice _physicalDevice);
	//bool checkDeviceExtensionSupport(VkPhysicalDevice _physicalDevice);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice _physicalDevice); //TO COMMENT
	//SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice _physicalDevice); // TO COMMENT


	// VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availableFormats);
	// VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	// VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	//std::vector<char> readFile(const std::string& fileName);
	// void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	// uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void recordCommandBuffer(VkCommandBuffer _commandBuffer, uint32_t imageIndex);
	void recreateSwapChain();
	void drawFrame();
	// static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	// 	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	// 	VkDebugUtilsMessageTypeFlagsEXT messageType,
	// 	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	// 	void* pUserData);

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto app = reinterpret_cast<TriangleApp*>(glfwGetWindowUserPointer(window));
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

	// std::vector<Vertex> triangle = {
	// 	{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	// 	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	// 	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	// };
};

#endif //TRIANGLEAPP_H