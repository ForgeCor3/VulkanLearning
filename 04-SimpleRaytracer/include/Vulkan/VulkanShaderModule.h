#ifndef VULKANSHADERMODULE_H
#define VULKANSHADERMODULE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>

#include "GlobUtils.h"

class VulkanShaderModule final
{
public:
    DISABLE_COPY_AND_MOVE(VulkanShaderModule)
    
    VulkanShaderModule() = delete;
    VulkanShaderModule(const VkDevice& device, const std::string filePath);
    ~VulkanShaderModule();

private:
    std::vector<char> readFile(std::string filePath);

    const VkDevice& device;
    VkShaderModule shaderModule;
};

#endif // VULKANSHADERMODULE_H