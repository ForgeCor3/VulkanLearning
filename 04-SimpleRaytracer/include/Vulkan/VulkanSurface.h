#ifndef VULKANSURFACE_H
#define VULKANSURFACE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "GlobUtils.h"
#include "Vulkan/VulkanInstance.h"

class VulkanSurface final
{
public:
    DISABLE_COPY_AND_MOVE(VulkanSurface)

    VulkanSurface() = delete;
    VulkanSurface(VulkanInstance* instance);
    ~VulkanSurface();

private:
    VkInstance instance;
    VkSurfaceKHR surface;
};


#endif // VULKANSURFACE_H