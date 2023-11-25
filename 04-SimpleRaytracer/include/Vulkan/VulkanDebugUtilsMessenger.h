#ifndef VULKANDEBUGUTILSMESSENGER_H
#define VULKANDEBUGUTILSMESSENGER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

#include "GlobUtils.h"

class VulkanDebugUtilsMessenger final
{
public:
    DISABLE_COPY_AND_MOVE(VulkanDebugUtilsMessenger)

    VulkanDebugUtilsMessenger();
    VulkanDebugUtilsMessenger(VkInstance& instance);
    ~VulkanDebugUtilsMessenger();

    void setDebugUtilsMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugUtilsMessengerCreateInfoEXT);

private:
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT pDebugMessenger, const VkAllocationCallbacks* pAllocator);

    VkInstance* instance;
    VkDebugUtilsMessengerEXT debugUtilsMessengerEXT = VK_NULL_HANDLE;
};

#endif // VULKANDEBUGUTILSMESSENGER_H