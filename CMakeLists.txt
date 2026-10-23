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
    SYSTEM
)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG 1.0.3
    SYSTEM
)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.93.0
    SYSTEM
)

FetchContent_MakeAvailable(glfw glm imgui)

add_library(imgui STATIC)

target_include_directories(
    imgui
    SYSTEM
    PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends
)

target_sources(
    imgui
    PRIVATE
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(
        ${PROJECT_NAME}
        PRIVATE
            -pedantic
            -Wall
            -Wextra
            -Wcast-align
            -Wcast-qual
            -Wctor-dtor-privacy
            -Wdisabled-optimization
            -Wformat=2
            -Winit-self
            -Wlogical-op
            -Wmissing-declarations
            -Wmissing-include-dirs
            -Wnoexcept
            -Wold-style-cast
            -Woverloaded-virtual
            -Wredundant-decls
            -Wshadow
            -Wsign-conversion
            -Wsign-promo
            -Wstrict-null-sentinel
            -Wstrict-overflow=5
            -Wswitch-default
            -Wundef
            -Werror
            -Wno-unused
    )
endif()

target_sources(
    ${PROJECT_NAME}
    PRIVATE
        ./src/main.cpp
        ./src/Application.cpp
        ./src/Window.cpp
        ./src/Camera.cpp
        ./src/Renderer.cpp
        ./src/Shader.cpp
        ./src/VoxelLogic.cpp
        ./src/Clock.cpp
        ./glad/src/gl.c
)

target_include_directories(${PROJECT_NAME} PRIVATE ./glad/include)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::OpenGL glfw glm imgui)
