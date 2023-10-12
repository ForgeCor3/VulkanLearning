#ifndef TRIANGLEAPP_H
#define TRIANGLEAPP_H

#include "extensions.h"

#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <optional>
#include <set>

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

	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();

	void pickPhysicalDevice();
	bool isPhysicalDeviceSuitable(VkPhysicalDevice _physicalDevice);
	bool checkDeviceExtensionSupport(VkPhysicalDevice _physicalDevice);

	void createLogicalDevice();
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice _physicalDevice);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice _physicalDevice);

	void createSurface();
	
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void setupDebugMessenger();

	GLFWwindow* window;

	VkInstance instance;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice logicalDevice;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkSurfaceKHR surface;
	VkDebugUtilsMessengerEXT debugMessenger;
};

#endif //TRIANGLEAPP_H