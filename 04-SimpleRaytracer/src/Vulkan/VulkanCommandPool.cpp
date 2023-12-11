#include "Vulkan/VulkanCommandPool.h"

VulkanCommandPool::VulkanCommandPool(const VkDevice& device, const uint32_t queueFamilyIndex) : device(device)
{
    VkCommandPoolCreateInfo commandPoolCreateInfo {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;

    if(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS)
        throw std::runtime_error("Failed to create command pool.");
}

VulkanCommandPool::~VulkanCommandPool() { vkDestroyCommandPool(device, commandPool, nullptr); }

VkCommandPool& VulkanCommandPool::getCommandPool() { return commandPool; }