#include "Vulkan/VulkanDevice.h"

VulkanDevice::VulkanDevice(VkInstance& instance)
{
    this->instance = &instance;

    selectPhysicalDevice();
}

VulkanDevice::~VulkanDevice()
{
    vkDestroyDevice(device, nullptr);
}

std::optional<uint32_t> VulkanDevice::findQueue(VkQueueFlags queueFlags, const VkPhysicalDevice physicalDevice)
{
    std::vector<VkQueueFamilyProperties> availableQueueFamilies;
    EnumerateVector(vkGetPhysicalDeviceQueueFamilyProperties, physicalDevice, availableQueueFamilies);

    std::optional<uint32_t> result;

    uint32_t index = 0;
    for(const VkQueueFamilyProperties& queueFamily : availableQueueFamilies)
    {
        if(queueFamily.queueFlags & queueFlags)
        {
            result = index;
            break;
        }

        index++;
    }
    
    return result;
}

void VulkanDevice::selectPhysicalDevice()
{
    std::vector<VkPhysicalDevice> availablePhysicalDevices;
    EnumerateVector(vkEnumeratePhysicalDevices, *instance, availablePhysicalDevices);
    if(availablePhysicalDevices.empty())
        throw std::runtime_error("Failed to find GPUs with supported vulkan.");

    physicalDevice = findSuitablePhysicalDevice(availablePhysicalDevices);
}

VkPhysicalDevice VulkanDevice::findSuitablePhysicalDevice(const std::vector<VkPhysicalDevice> availablePhysicalDevices)
{
    std::multimap<uint32_t, VkPhysicalDevice> physicalDeviceScores;

    uint32_t graphicsQueueFamilyIndex;

    for(const VkPhysicalDevice& physicalDevice : availablePhysicalDevices)
    {
        uint32_t currentPhysicalDeviceScore = 0;

        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

        if(physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            currentPhysicalDeviceScore += 10;
        if(physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
            currentPhysicalDeviceScore += 5;
        if(physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU)
            currentPhysicalDeviceScore += 1;
        
        if(!findQueue(VK_QUEUE_GRAPHICS_BIT, physicalDevice).has_value())
            currentPhysicalDeviceScore = 0;

        physicalDeviceScores.emplace(currentPhysicalDeviceScore, physicalDevice);
    }

    return physicalDeviceScores.rbegin()->second;
}

void VulkanDevice::setupLogicalDevice()
{
    graphicsQueueFamilyIndex = findQueue(VK_QUEUE_GRAPHICS_BIT, physicalDevice).value();
    float graphicsQueuePriority = 1.0f;

    VkDeviceQueueCreateInfo graphicsQueueCreateInfo {};
    graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    graphicsQueueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
    graphicsQueueCreateInfo.queueCount = 1;
    graphicsQueueCreateInfo.pQueuePriorities = &graphicsQueuePriority;

    VkPhysicalDeviceFeatures physicalDeviceFeatures {};

    VkDeviceCreateInfo deviceCreateInfo {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = &graphicsQueueCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
    deviceCreateInfo.enabledExtensionCount = 0;

    if(!vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS)
        throw std::runtime_error("Failed to create device.");

    vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
}