#include "Vulkan/VulkanDevice.h"

VulkanDevice::VulkanDevice(VulkanInstance& instance, VulkanSurface& surface)
{
    this->instance = &instance;
    this->surface = surface.getSurface();

    selectPhysicalDevice(instance);
    setupLogicalDevice();
}

VulkanDevice::~VulkanDevice() { vkDestroyDevice(device, nullptr); }

VkDevice& VulkanDevice::getDevice() { return device; }
VkPhysicalDevice& VulkanDevice::getPhysicalDevice() { return physicalDevice; }

VkSurfaceKHR& VulkanDevice::getSurface() { return surface; }
GLFWwindow* VulkanDevice::getWindow() { return instance->getWindow(); }

uint32_t VulkanDevice::getGraphicsQueueFamilyIndex() { return graphicsQueueFamilyIndex; }
uint32_t VulkanDevice::getPresentQueueFamilyIndex() { return presentQueueFamilyIndex; }

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

        if(!findQueue(VK_QUEUE_GRAPHICS_BIT, physicalDevice).has_value() || !checkDeviceExtensionsSupport(physicalDevice))
        {
            currentPhysicalDeviceScore = 0;
            physicalDeviceScores.emplace(currentPhysicalDeviceScore, physicalDevice);
            continue;
        }

        if(physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            currentPhysicalDeviceScore += 10;
        if(physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
            currentPhysicalDeviceScore += 5;
        if(physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU)
            currentPhysicalDeviceScore += 1;

        physicalDeviceScores.emplace(currentPhysicalDeviceScore, physicalDevice);
    }

    return physicalDeviceScores.rbegin()->second;
}

bool VulkanDevice::checkDeviceExtensionsSupport(VkPhysicalDevice physicalDevice)
{
    std::vector<VkExtensionProperties> extensionProperties;
    EnumerateVector(vkEnumerateDeviceExtensionProperties, physicalDevice, extensionProperties);

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for(const auto extension : extensionProperties)
        requiredExtensions.erase(extension.extensionName);

    return requiredExtensions.empty();
}

void VulkanDevice::setupLogicalDevice()
{
    std::vector<VkQueueFamilyProperties> availableQueueFamilies;
    EnumerateVector(vkGetPhysicalDeviceQueueFamilyProperties, physicalDevice, availableQueueFamilies);

    for(auto it = availableQueueFamilies.begin(); it != availableQueueFamilies.end(); ++it)
    {
        VkBool32 pSupported = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, static_cast<uint32_t>(it - availableQueueFamilies.begin()), surface, &pSupported);
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
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
    
    if(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS)
        throw std::runtime_error("Failed to create device.");

    vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
    vkGetDeviceQueue(device, presentQueueFamilyIndex, 0, &presentQueue);
}