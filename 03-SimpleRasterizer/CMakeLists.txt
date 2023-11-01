cmake_minimum_required(VERSION 3.27.0)

set (CMAKE_CXX_STANDARD 17)

project(03_SimpleRasterizer)

include_directories(03_SimpleRasterizer
    include/
    ../include/
    ../include/glfw3
    ../include/glm
    ../include/vulkan
)

link_directories(03_SimpleRasterizer
    ../lib/glfw3
    ../lib/vulkan    
)

add_executable(03_SimpleRasterizer
    src/Utility.cpp
    src/VulkanInitialization.cpp
    src/Extensions.cpp
    src/SimpleRasterizer.cpp
    src/main.cpp
)

target_link_libraries(03_SimpleRasterizer
    glfw3
    vulkan-1
)

add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/shaders
    ${PROJECT_BINARY_DIR}/shaders
    COMMENT "Copying shaders into binary directory."
    )
add_dependencies(03_SimpleRasterizer copy_shaders)