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
    src/Vulkan/VulkanGraphicsPipeline.cpp
    src/Vulkan/VulkanShaderModule.cpp
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

add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/shaders
    ${PROJECT_BINARY_DIR}/shaders
    COMMENT "Copying shaders into binary directory."
    )

add_dependencies(04-SimpleRaytracer copy_shaders)