cmake_minimum_required(VERSION 3.10)
project(Chip8Emulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Manually specify SDL2 paths
set(SDL2_PATH "${CMAKE_SOURCE_DIR}/external/SDL2")
set(SDL2_INCLUDE_DIR "${SDL2_PATH}/include")
set(SDL2_LIBRARY "${SDL2_PATH}/lib/x64/SDL2.lib")
set(SDL2_MAIN_LIBRARY "${SDL2_PATH}/lib/x64/SDL2main.lib")

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${SDL2_INCLUDE_DIR})

# Source files
set(SOURCES
    src/main.cpp
    src/chip8.cpp
    src/renderer.cpp
)

# Add executable
add_executable(chip8emulator ${SOURCES})

# Link libraries
target_link_libraries(chip8emulator ${SDL2_LIBRARY} ${SDL2_MAIN_LIBRARY})

# Copy SDL2.dll to build directory
add_custom_command(TARGET chip8emulator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${SDL2_PATH}/lib/x64/SDL2.dll"
    $<TARGET_FILE_DIR:chip8emulator>)

# Set the startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT chip8emulator)