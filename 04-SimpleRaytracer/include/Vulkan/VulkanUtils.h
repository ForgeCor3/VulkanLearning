#ifndef VULKANUTILS_H
#define VULKANUTILS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

template <typename T>
inline void EnumerateVector(VkResult (*enumerationFunction)(uint32_t*, T*), std::vector<T>& data)
{
    uint32_t count = 0;
    enumerationFunction(&count, nullptr);
    data.resize(count);
    enumerationFunction(&count, data.data());
}

template <typename T>
inline void EnumerateVector(VkResult (*enumerationFunction)(VkInstance, uint32_t*, T*), VkInstance& instance, std::vector<T>& data)
{
    uint32_t count = 0;
    enumerationFunction(instance, &count, nullptr);
    data.resize(count);
    enumerationFunction(instance, &count, data.data());
}

#endif // VULKANUTILS_H