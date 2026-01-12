cmake_minimum_required(VERSION 3.15)
project(8bit-synth)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

# Find SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# ImGui sources
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
file(GLOB IMGUI_SOURCES 
    ${IMGUI_DIR}/*.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# Source files
file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/controller/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/view/*.cpp
)

add_executable(8bit-synth 
    ${SOURCES}
    ${IMGUI_SOURCES}
)

target_link_libraries(8bit-synth 
    ${SDL2_LIBRARIES}
)

# Copy build to root for easy access
add_custom_command(TARGET 8bit-synth POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:8bit-synth> ${CMAKE_CURRENT_SOURCE_DIR}/
)

# Copy assets to build directory
add_custom_command(TARGET 8bit-synth POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_CURRENT_SOURCE_DIR}/assets 
    ${CMAKE_CURRENT_BINARY_DIR}/assets
)

# Test executable
add_executable(test_synth
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_synth.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Effects.cpp
)

target_include_directories(test_synth PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Copy test executable to root
add_custom_command(TARGET test_synth POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:test_synth> ${CMAKE_CURRENT_SOURCE_DIR}/
)
