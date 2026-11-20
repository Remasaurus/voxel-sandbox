cmake_minimum_required(VERSION 3.25)
project(voxel-sandbox VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME})

#
# Depedencies
#
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

#
# Dear ImGui
#
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

#
# Executable
#
target_sources(
    ${PROJECT_NAME}
    PRIVATE
        src/main.cpp
        src/Application.cpp
        src/Clock.cpp
        src/Window.cpp
        src/Brush.cpp
        src/render/Camera.cpp
        src/render/Renderer.cpp
        src/render/Framebuffer.cpp
        src/render/debug.cpp
        src/render/Shader.cpp
        src/voxel/VoxelLogic.cpp
        src/voxel/VoxFileParser.cpp
        src/voxel/VoxelMap.cpp
        glad/src/gl.c
)

target_include_directories(${PROJECT_NAME} PRIVATE glad/include)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::OpenGL glfw glm imgui)

#
# Add warnings
#
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX /permissive-)
else()
    target_compile_options(
        ${PROJECT_NAME}
        PRIVATE
            -pedantic
            -Wall
            -Wextra
            -Wcast-align
            -Wcast-qual
            $<$<COMPILE_LANGUAGE:CXX>:-Wctor-dtor-privacy>
            -Wdisabled-optimization
            -Wformat=2
            -Winit-self
            -Wlogical-op
            -Wmissing-declarations
            -Wmissing-include-dirs
            $<$<COMPILE_LANGUAGE:CXX>:-Wnoexcept>
            $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>
            $<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>
            -Wredundant-decls
            -Wshadow
            -Wsign-conversion
            $<$<COMPILE_LANGUAGE:CXX>:-Wsign-promo>
            $<$<COMPILE_LANGUAGE:CXX>:-Wstrict-null-sentinel>
            -Wstrict-overflow=2
            -Wswitch-default
            -Wundef
            -Werror
            -Wno-unused
    )
endif()

#
# Debug
#
add_compile_options(
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Debug>:-fsanitize=address>
    $<$<CONFIG:Debug>:-fsanitize=undefined>
    $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
)
add_link_options($<$<CONFIG:Debug>:-fsanitize=address,undefined>)
add_compile_definitions(
    $<$<CONFIG:Debug>:_GLIBCXX_ASSERTIONS>
    $<$<CONFIG:Debug>:_GLIBCXX_DEBUG>
    $<$<CONFIG:Debug>:_GLIBCXX_DEBUG_PEDANTIC>
    # $<$<CONFIG:Debug>:_GLIBCXX_DEBUG_BACKTRACE>
)

#
# Release
#
add_compile_options(
    # Release mode
    $<$<NOT:$<CONFIG:Debug>>:-march=native>
    $<$<NOT:$<CONFIG:Debug>>:-mtune=native>
)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error LANGUAGES CXX)

if(ipo_supported)
    message(STATUS "IPO/LTO enabled")
    set_property(
        TARGET ${PROJECT_NAME}
        PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
    )
else()
    message(STATUS "IPO/LTO not supported: ${ipo_error}")
endif()
