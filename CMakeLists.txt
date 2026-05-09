cmake_minimum_required(VERSION 4.3 FATAL_ERROR)

set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C language needs to be enabled for LLVM
project(example
    LANGUAGES
        C
        CXX
)

add_library(config INTERFACE)

target_compile_features(config INTERFACE "cxx_std_${CMAKE_CXX_STANDARD}")

add_subdirectory(src)
