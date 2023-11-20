#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H

#include <stdexcept>
#include <vector>

#include "GlobUtils.h"
#include "VulkanUtils.h"
#include "Window.h"

class VulkanInstance final
{
private:
    DISABLE_COPY_AND_MOVE(VulkanInstance)

    void checkValidationLayerSupport(std::vector<const char*>& validationLayers);

    VkInstance instance;
    std::vector<VkExtensionProperties> extensions;
    std::vector<VkLayerProperties> layers;

public:
    VulkanInstance() = delete;
    VulkanInstance(const class Window& window, std::vector<const char*>& validationLayers);
    
    ~VulkanInstance();

    VkInstance& getInstance() { return instance; }
};

#endif // VULKANINSTANCE_H