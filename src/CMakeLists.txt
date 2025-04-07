cmake_minimum_required(VERSION 3.31.5)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(renderer VERSION 0.1)
add_executable(renderer main.cpp)

# Ensure glfw3 and opengl exist
find_package(glfw3 3.4.0 REQUIRED)
find_package(assimp REQUIRED)
find_package(OpenGL REQUIRED)

# Include GLAD
add_library(glad STATIC glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

# Adding logging library
add_library(log STATIC log/log.cpp)

# Adding texture, mesh and model libs
add_library(texture STATIC textures/texture.cpp)
add_library(mesh STATIC model/mesh/mesh.cpp)
add_library(model STATIC model/model.cpp)

# Link libraries
target_link_libraries(renderer PRIVATE assimp model mesh log texture glad glfw GL X11 pthread Xrandr Xi dl)
