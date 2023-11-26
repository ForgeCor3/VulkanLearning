#include "Vulkan/VulkanDevice.h"

VulkanDevice::VulkanDevice(VulkanInstance& instance, VulkanSurface& surface)
{
    selectPhysicalDevice(instance);
    setupLogicalDevice(surface);
}

VulkanDevice::~VulkanDevice()
{
    vkDestroyDevice(device, nullptr);
}

std::optional<uint32_t> VulkanDevice::findQueue(const VkQueueFlags queueFlags, const VkPhysicalDevice physicalDevice)
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

void VulkanDevice::selectPhysicalDevice(VulkanInstance& instance)
{
    std::vector<VkPhysicalDevice> availablePhysicalDevices;
    EnumerateVector(vkEnumeratePhysicalDevices, instance.getInstance(), availablePhysicalDevices);

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

void VulkanDevice::setupLogicalDevice(VulkanSurface& surface)
{
    std::vector<VkQueueFamilyProperties> availableQueueFamilies;
    EnumerateVector(vkGetPhysicalDeviceQueueFamilyProperties, physicalDevice, availableQueueFamilies);

    for(auto it = availableQueueFamilies.begin(); it != availableQueueFamilies.end(); ++it)
    {
        VkBool32 pSupported = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, static_cast<uint32_t>(it - availableQueueFamilies.begin()), surface.getSurface(), &pSupported);
        if(pSupported)
        {
            presentQueueFamilyIndex = static_cast<uint32_t>(it - availableQueueFamilies.begin());
            break;
        }
    }
    
    graphicsQueueFamilyIndex = findQueue(VK_QUEUE_GRAPHICS_BIT, physicalDevice).value();

    std::set<uint32_t> uniqueQueueFamilies =
    {
        presentQueueFamilyIndex,
        graphicsQueueFamilyIndex
    };

    float queuePriority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
    for(uint32_t queueFamilyIndex : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo deviceQueueCreateInfo {};
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndex;
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.pQueuePriorities = &queuePriority;
        deviceQueueCreateInfos.push_back(deviceQueueCreateInfo);
    }

    VkPhysicalDeviceFeatures physicalDeviceFeatures {};

    VkDeviceCreateInfo deviceCreateInfo {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
    deviceCreateInfo.enabledExtensionCount = 0;
    
    if(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS)
        throw std::runtime_error("Failed to create device.");

    vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
    vkGetDeviceQueue(device, presentQueueFamilyIndex, 0, &presentQueue);
}