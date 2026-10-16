cmake_minimum_required(VERSION 3.25)
project(voxel-sandbox LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME})

find_package(OpenGL REQUIRED)

include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG 3.5.1
)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG 1.0.3
)

FetchContent_MakeAvailable(glfw glm)

target_sources(${PROJECT_NAME}
PRIVATE
	./src/main.cpp

    ./glad/src/gl.c
)

target_include_directories(${PROJECT_NAME}
PRIVATE
    ./glad/include
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
	OpenGL::OpenGL
    glfw
)
