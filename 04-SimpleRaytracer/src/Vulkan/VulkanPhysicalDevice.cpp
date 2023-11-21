#include "Vulkan/VulkanPhysicalDevice.h"

VulkanPhysicalDevice::VulkanPhysicalDevice(VkInstance& instance)
{
    this->instance = &instance;

    selectPhysicalDevice();
}

void VulkanPhysicalDevice::selectPhysicalDevice()
{
    std::vector<VkPhysicalDevice> availablePhysicalDevices;
    EnumerateVector(vkEnumeratePhysicalDevices, *instance, availablePhysicalDevices);

    if(availablePhysicalDevices.empty()) throw std::runtime_error("Failed to find GPUs with supported vulkan.");
}