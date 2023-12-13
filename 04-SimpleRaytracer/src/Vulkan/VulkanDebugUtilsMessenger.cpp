#include "Vulkan/VulkanDebugUtilsMessenger.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT		messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT				messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void*										pUserData)
{
	switch (messageSeverity)
	{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			std::cerr << "[VERBOSE, ";
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			std::cerr << "[WARNING, ";
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			std::cerr << "[ERROR, ";
			break;
	}

	switch (messageType)
	{
		case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
			std::cerr << "GENERAL]: ";
			break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
			std::cerr << "VALIDATION]: ";
			break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
			std::cerr << "PERFORMANCE]: ";
			break;
	}

	std::cerr << pCallbackData->pMessage << std::endl;
	std::cerr << "----------" << std::endl;
	return false;
}

VulkanDebugUtilsMessenger::VulkanDebugUtilsMessenger() {}

VulkanDebugUtilsMessenger::VulkanDebugUtilsMessenger(VkInstance& instance)
{
	this->instance = &instance;

	VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo{};
	setDebugUtilsMessengerCreateInfo(debugUtilsMessengerCreateInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &debugUtilsMessengerCreateInfo, nullptr, &debugUtilsMessengerEXT) != VK_SUCCESS)
		throw std::runtime_error("Failed to set up debug messenger.");
}

VulkanDebugUtilsMessenger::~VulkanDebugUtilsMessenger()
{
	DestroyDebugUtilsMessengerEXT(*instance, debugUtilsMessengerEXT, nullptr);
}

VkResult VulkanDebugUtilsMessenger::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto fn = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	return fn == nullptr ? VK_ERROR_EXTENSION_NOT_PRESENT : fn(instance, pCreateInfo, pAllocator, pDebugMessenger);
}

void VulkanDebugUtilsMessenger::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT pDebugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto fn = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (fn != nullptr)
		return fn(instance, pDebugMessenger, pAllocator);
}

void VulkanDebugUtilsMessenger::setDebugUtilsMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugUtilsMessengerCreateInfo)
{
	debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugUtilsMessengerCreateInfo.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugUtilsMessengerCreateInfo.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugUtilsMessengerCreateInfo.pfnUserCallback = debug_callback;
	debugUtilsMessengerCreateInfo.pUserData = nullptr;
}