cmake_minimum_required(VERSION 3.27.0)

project(01_VulkanTest)

include_directories(01_VulkanTest
    ../include/
    ../include/glfw3
    ../include/glm
    ../include/vulkan
)

link_directories(01_VulkanTest
    ../lib/glfw3
    ../lib/vulkan    
)

add_executable(01_VulkanTest
    main.cpp
)

target_link_libraries(01_VulkanTest
    glfw3
    vulkan-1
)