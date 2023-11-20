#ifndef VULKANDEBUGUTILSMESSENGER_H
#define VULKANDEBUGUTILSMESSENGER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

class VulkanDebugUtilsMessenger final
{
private:
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT pDebugMessenger, const VkAllocationCallbacks* pAllocator);

    VkDebugUtilsMessengerEXT debugUtilsMessengerEXT;

public:
    VulkanDebugUtilsMessenger() = delete;
    VulkanDebugUtilsMessenger(VkInstance& instance);

    void terminate(VkInstance& instance);

    ~VulkanDebugUtilsMessenger() {};
};

#endif // VULKANDEBUGUTILSMESSENGER_H