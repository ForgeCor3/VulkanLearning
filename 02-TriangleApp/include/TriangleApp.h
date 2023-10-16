#ifndef TRIANGLEAPP_H
#define TRIANGLEAPP_H

#include "extensions.h"

#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <optional>
#include <set>
#include <limits>
#include <algorithm>
#include <fstream>

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

const uint32_t WIDTH = 1280;
const uint32_t HEIGHT = 720;

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

	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanUp();

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
};

#endif //TRIANGLEAPP_H