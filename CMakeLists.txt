cmake_minimum_required(VERSION 3.12)
project(SnakeConsole LANGUAGES CXX VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Platform-specific global settings
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
elseif(UNIX AND NOT APPLE)
    # Linux
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
elseif(APPLE)
    # macOS
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment target")
endif()

# Executable
add_executable(snake
    src/main.cpp
    src/platform.cpp
    src/render.cpp
    src/util.cpp
    src/ui.cpp
    src/game.cpp
    src/score.cpp
)

target_include_directories(snake PRIVATE include)

# Platform-specific target settings
if(WIN32)
    target_compile_definitions(snake PRIVATE _WIN32_WINNT=0x0601)
endif()

# Compiler-specific options
if(MSVC)
    target_compile_options(snake PRIVATE /W4 /permissive-)
else()
    target_compile_options(snake PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Build type specific options
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(snake PRIVATE DEBUG)
    if(MSVC)
        target_compile_options(snake PRIVATE /Od /Zi)
    else()
        target_compile_options(snake PRIVATE -g -O0)
    endif()
else()
    if(MSVC)
        target_compile_options(snake PRIVATE /O2)
    else()
        target_compile_options(snake PRIVATE -O3 -DNDEBUG)
    endif()
endif()

# Installation rules (cross-platform)
install(TARGETS snake
    RUNTIME DESTINATION bin
    COMPONENT Runtime
)

