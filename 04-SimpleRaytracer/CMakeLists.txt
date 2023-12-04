cmake_minimum_required(VERSION 3.27.0)

project(04-SimpleRaytracer)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES 
    src/Vulkan/VulkanInstance.cpp
    src/Vulkan/VulkanDebugUtilsMessenger.cpp
    src/Vulkan/VulkanSurface.cpp
    src/Vulkan/VulkanDevice.cpp
    src/Vulkan/VulkanSwapChain.cpp
    src/Vulkan/VulkanImageView.cpp
    src/Window.cpp
    src/Application.cpp
    src/main.cpp
)

set(HEADERPATHS
    include/
    ../include/glfw3/
    ../include/vulkan/
)

link_directories(04-SimpleRaytracer
    ../lib/glfw3
    ../lib/vulkan
)

include_directories(04-SimpleRaytracer ${HEADERPATHS})
add_executable(04-SimpleRaytracer ${SOURCES})

target_link_libraries(04-SimpleRaytracer
    glfw3
    vulkan-1
)