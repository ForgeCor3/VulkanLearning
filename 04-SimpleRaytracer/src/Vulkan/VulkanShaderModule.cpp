#include "Vulkan/VulkanShaderModule.h"

VulkanShaderModule::VulkanShaderModule(const VkDevice& device, const std::string filePath) : device(device)
{
    std::vector<char> shaderCode = readFile(filePath);

    VkShaderModuleCreateInfo shaderModuleCreateInfo { };
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.flags = 0;
    shaderModuleCreateInfo.codeSize = shaderCode.size();
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());
    
    if(vkCreateShaderModule(this->device, &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS)
        throw std::runtime_error("Failed to create shader");
}

VulkanShaderModule::~VulkanShaderModule() { vkDestroyShaderModule(this->device, shaderModule, nullptr); }

VkPipelineShaderStageCreateInfo VulkanShaderModule::createPipelineShaderStageCreateInfo(VkShaderStageFlagBits stage) const
{
    VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo {};
    pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineShaderStageCreateInfo.stage = stage;
    pipelineShaderStageCreateInfo.module = shaderModule;
    pipelineShaderStageCreateInfo.pName = "main";

    return pipelineShaderStageCreateInfo;
}

std::vector<char> VulkanShaderModule::readFile(std::string filePath)
{
    std::ifstream ifs(filePath, std::ios::ate | std::ios::binary);
    if(!ifs.is_open())
        throw std::runtime_error("Failed to open shader file " + filePath);

    size_t fileSize = (size_t)ifs.tellg();
    std::vector<char> buffer(fileSize);

    ifs.seekg(0);
    ifs.read(buffer.data(), fileSize);

    ifs.close();
    return buffer;
}