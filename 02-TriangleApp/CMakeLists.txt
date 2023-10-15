cmake_minimum_required(VERSION 3.27.0)

set (CMAKE_CXX_STANDARD 17)

project(02_TriangleApp)

include_directories(02_TriangleApp
    include/
    ../include/
    ../include/glfw3
    ../include/glm
    ../include/vulkan
)

link_directories(02_TriangleApp
    ../lib/glfw3
    ../lib/vulkan    
)

add_executable(02_TriangleApp
    src/extensions.cpp
    src/TriangleApp.cpp
    src/main.cpp
)

target_link_libraries(02_TriangleApp
    glfw3
    vulkan-1
)

add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/shaders
    ${PROJECT_BINARY_DIR}/shaders
    COMMENT "Copying shaders into binary directory."
    )
add_dependencies(02_TriangleApp copy_shaders)