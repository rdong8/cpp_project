cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

set(CMAKE_COLOR_DIAGNOSTICS ON)

# https://gitlab.kitware.com/cmake/cmake/-/blob/master/Help/dev/experimental.rst?ref_type=heads
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(cpp_project
        VERSION 0.0.1
        DESCRIPTION "Template for a modern C++ project using CMake."
        HOMEPAGE_URL "https://github.com/rdong8/cpp_project"
        LANGUAGES CXX
)

add_library(config INTERFACE)

target_compile_features(config INTERFACE "cxx_std_${CMAKE_CXX_STANDARD}")

# Use libc++
target_compile_options(config INTERFACE "-stdlib=libc++")
target_link_options(config INTERFACE "-lc++abi;-stdlib=libc++")

add_subdirectory(src)
