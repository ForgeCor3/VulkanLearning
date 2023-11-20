#ifndef VULKANDEBUGUTILSMESSENGER_H
#define VULKANDEBUGUTILSMESSENGER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

#include "GlobUtils.h"

class VulkanDebugUtilsMessenger final
{
private:
    DISABLE_COPY_AND_MOVE(VulkanDebugUtilsMessenger)

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT pDebugMessenger, const VkAllocationCallbacks* pAllocator);

    VkInstance* instance;
    VkDebugUtilsMessengerEXT debugUtilsMessengerEXT;

public:
    VulkanDebugUtilsMessenger() = delete;
    VulkanDebugUtilsMessenger(VkInstance& instance);

    ~VulkanDebugUtilsMessenger();
};

#endif // VULKANDEBUGUTILSMESSENGER_H