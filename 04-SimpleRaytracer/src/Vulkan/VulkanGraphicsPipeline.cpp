#include "Vulkan/VulkanGraphicsPipeline.h"

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanDevice& device) : device(device.getDevice())
{
    const VulkanShaderModule vertexShader(this->device, "../shaders/vertexShader.spv");
    const VulkanShaderModule fragmentShader(this->device, "../shaders/fragmentShader.spv");
}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{

}