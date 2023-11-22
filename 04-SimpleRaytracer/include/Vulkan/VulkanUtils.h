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
inline void EnumerateVector(VkResult (*enumerationFunction)(VkInstance, uint32_t*, T*), const VkInstance& instance, std::vector<T>& data)
{
    uint32_t count = 0;
    enumerationFunction(instance, &count, nullptr);
    data.resize(count);
    enumerationFunction(instance, &count, data.data());
}

template <typename T>
inline void EnumerateVector(VkResult (*enumerationFunction)(VkPhysicalDevice, const char*, uint32_t*, T*), const VkPhysicalDevice& physicalDevice, std::vector<T>& data)
{
    uint32_t count = 0;
    enumerationFunction(physicalDevice, nullptr, &count, nullptr);
    data.resize(count);
    enumerationFunction(physicalDevice, nullptr, &count, data.data());
}

template <typename T>
inline void EnumerateVector(void (*enumerationFunction)(VkPhysicalDevice, uint32_t*, T*), const VkPhysicalDevice& physicalDevice, std::vector<T>& data)
{
    uint32_t count = 0;
    enumerationFunction(physicalDevice, &count, nullptr);
    data.resize(count);
    enumerationFunction(physicalDevice, &count, data.data());
}

#endif // VULKANUTILS_H