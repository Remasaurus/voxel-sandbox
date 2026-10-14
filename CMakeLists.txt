cmake_minimum_required(VERSION 3.25)
project(voxel-sandbox LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME})

find_package(OpenGL REQUIRED)

target_sources(${PROJECT_NAME}
PRIVATE
	./src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
	OpenGL::OpenGL
)
