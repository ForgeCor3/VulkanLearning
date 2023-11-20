#include "Vulkan/VulkanDebugUtilsMessenger.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

VulkanDebugUtilsMessenger::VulkanDebugUtilsMessenger(VkInstance& instance)
{
    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT {};
    debugUtilsMessengerCreateInfoEXT.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugUtilsMessengerCreateInfoEXT.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugUtilsMessengerCreateInfoEXT.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugUtilsMessengerCreateInfoEXT.pfnUserCallback = debug_callback;
    debugUtilsMessengerCreateInfoEXT.pUserData = nullptr;

    if(CreateDebugUtilsMessengerEXT(instance, &debugUtilsMessengerCreateInfoEXT, nullptr, &debugUtilsMessengerEXT) != VK_SUCCESS)
        throw std::runtime_error("Failed to set up debug messenger.");
}

VkResult VulkanDebugUtilsMessenger::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto fn = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    return fn == nullptr ? VK_ERROR_EXTENSION_NOT_PRESENT : fn(instance, pCreateInfo, pAllocator, pDebugMessenger);
}

void VulkanDebugUtilsMessenger::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT pDebugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto fn = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if(fn != nullptr)
        return fn(instance, pDebugMessenger, pAllocator);
}