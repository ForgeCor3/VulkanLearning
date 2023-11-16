cmake_minimum_required(VERSION 3.27.0)

project(04-SimpleRaytracer)

link_directories(04-SimpleRaytracer
    ../lib/glfw3
)

include_directories(04-SimpleRaytracer
    include/
    ../include/
    ../include/glfw3/
)

add_executable(04-SimpleRaytracer
    src/Window.cpp
    src/Application.cpp
    src/main.cpp
)

target_link_libraries(04-SimpleRaytracer
    glfw3
)