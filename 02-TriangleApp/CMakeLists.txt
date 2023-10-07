cmake_minimum_required(VERSION 3.27.0)

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
    src/TriangleApp.cpp
    src/main.cpp
)

target_link_libraries(02_TriangleApp
    glfw3
    vulkan-1
)