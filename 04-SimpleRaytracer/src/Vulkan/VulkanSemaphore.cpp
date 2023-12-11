#include "Vulkan/VulkanSemaphore.h"

VulkanSemaphore::VulkanSemaphore(const VkDevice& device) : device(device)
{
    VkSemaphoreCreateInfo semaphoreCreateInfo {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphore) != VK_SUCCESS)
        throw std::runtime_error("Failed to create semaphore.");
}

VulkanSemaphore::~VulkanSemaphore() { vkDestroySemaphore(device, semaphore, nullptr); }

VkSemaphore& VulkanSemaphore::getSemaphore() { return semaphore; }