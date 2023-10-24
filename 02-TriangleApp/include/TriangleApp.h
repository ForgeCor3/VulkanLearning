#ifndef TRIANGLEAPP_H
#define TRIANGLEAPP_H

#include "extensions.h"

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

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const int MAX_FRAMES_IN_FLIGHT = 2;

class TriangleApp
{
public:
	void run();

private:
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		bool isComplete() {return graphicsFamily.has_value() && presentFamily.has_value();}
	};

	struct SwapChainSupportDetails
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

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto app = reinterpret_cast<TriangleApp*>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}

	void initVulkan();
	void mainLoop();
	void cleanUp();
	void cleanUpSwapChain();

	void createInstance();

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void setupDebugMessenger();

	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();

	void createSurface();

	void pickPhysicalDevice();
	bool isPhysicalDeviceSuitable(VkPhysicalDevice _physicalDevice);
	bool checkDeviceExtensionSupport(VkPhysicalDevice _physicalDevice);

	void createLogicalDevice();
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice _physicalDevice);
	
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice _physicalDevice);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	void createSwapChain();
	void createSwapChainImageViews();

	void createRenderPass();
	void createGraphicsPipeline();
	VkShaderModule createShaderModule(const std::vector<char>& shaderCode);

	std::vector<char> readFile(const std::string& fileName);

	void createFramebuffers();

	void createCommandPool();
	void createCommandBuffers();
	void recordCommandBuffer(VkCommandBuffer _commandBuffer, uint32_t imageIndex);

	void createSyncObjects();

	void drawFrame();

	void recreateSwapChain();

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
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	uint32_t currentFrame = 0;
	bool framebufferResized = false;

	const std::vector<Vertex> triangle = {
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};
};

#endif //TRIANGLEAPP_H