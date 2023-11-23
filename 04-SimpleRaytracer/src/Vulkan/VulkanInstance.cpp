#include "Vulkan/VulkanInstance.h"

VulkanInstance::VulkanInstance(const class Window& window, std::vector<const char*>& validationLayers)
{
    std::vector<const char*> extensions = window.getRequiredInstanceExtensions();

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

    if(!validationLayers.empty())
    {
        checkValidationLayerSupport(validationLayers);

        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

        debugUtilsMessenger.reset(new VulkanDebugUtilsMessenger());
        VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo {};
        debugUtilsMessenger.get()->setDebugUtilsMessengerCreateInfo(debugUtilsMessengerCreateInfo);

        instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugUtilsMessengerCreateInfo;
    }
    else
    {
        instanceCreateInfo.enabledLayerCount = 0;
        instanceCreateInfo.ppEnabledLayerNames = nullptr;
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
        instanceCreateInfo.pNext = nullptr;
    }
    
    if(vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS)
        throw std::runtime_error("Failed to create instance.");
}

 VulkanInstance::~VulkanInstance() { vkDestroyInstance(instance, nullptr); }

void VulkanInstance::checkValidationLayerSupport(std::vector<const char*>& validationLayers)
{
    std::vector<VkLayerProperties> availableLayers;
    EnumerateVector(vkEnumerateInstanceLayerProperties, availableLayers);

    for(const char* layer : validationLayers)
    {
        bool layerFound = false;
        for(const auto& layerProperties : availableLayers)
        {
            if(strcmp(layer, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if(!layerFound)
            throw std::runtime_error("Validation layers requested, but not available.");
    }
}