cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(cpp_project
        VERSION 0.0.1
        DESCRIPTION "Template for a modern C++ project using CMake."
        HOMEPAGE_URL "https://github.com/rdong8/cpp_project"
        LANGUAGES CXX
)

add_library(config INTERFACE)

target_compile_features(config INTERFACE "cxx_std_${CMAKE_CXX_STANDARD}")

add_subdirectory(src)
