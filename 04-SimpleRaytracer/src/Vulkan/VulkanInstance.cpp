#include "Vulkan/VulkanInstance.h"

VulkanInstance::VulkanInstance(const class Window& window)
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    extensions.resize(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    VkApplicationInfo applicationInfo {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = "applicationName";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = "No Engine";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion  = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instanceCreateInfo {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledLayerCount = 0; // TO CHANGE
    instanceCreateInfo.ppEnabledLayerNames = nullptr; // TO CHANGE
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(window.getRequiredInstanceExtensions().size());
    instanceCreateInfo.ppEnabledExtensionNames = window.getRequiredInstanceExtensions().data();

    if(vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS)
        throw std::runtime_error("Failed to create instance.");
}

void VulkanInstance::terminate() { vkDestroyInstance(instance, nullptr); }