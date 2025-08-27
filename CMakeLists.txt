cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

if ("$<NOT:$<CXX_COMPILER_ID:Clang,GCC>>")
    message(FATAL_ERROR "Invalid compiler. Must be either Clang or GCC.")
endif ()

set(CMAKE_COLOR_DIAGNOSTICS ON)
# https://gitlab.kitware.com/cmake/cmake/-/blob/master/Help/dev/experimental.rst?ref_type=heads
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Force libc++ globally so CMake can detect std modules with Clang
set(CMAKE_CXX_FLAGS_INIT "-stdlib=libc++ ${CMAKE_CXX_FLAGS_INIT}")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-stdlib=libc++ -lc++abi ${CMAKE_EXE_LINKER_FLAGS_INIT}")
# Propagate to try_compile checks invoked during configure
set(CMAKE_TRY_COMPILE_PLATFORM_VARIABLES
    CMAKE_CXX_FLAGS_INIT
    CMAKE_EXE_LINKER_FLAGS_INIT
)

# Generate compile_commands.json to make it easier to work with clang based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(cpp_project
        VERSION 0.0.1
        DESCRIPTION "Template for a modern C++ project using CMake."
        HOMEPAGE_URL "https://github.com/rdong8/cpp_project"
        LANGUAGES CXX
)

add_library(config INTERFACE)

target_compile_features(config INTERFACE "cxx_std_${CMAKE_CXX_STANDARD}")

include(cmake/CompileOptions.cmake)
set_project_compile_options(config)

include(cmake/CompileWarnings.cmake)
set_project_warnings(config)

include(cmake/Sanitizers.cmake)
set_project_sanitizers(config)

include(cmake/StandardLibrary.cmake)
set_project_stdlib(config)

include(cmake/InterproceduralOptimization.cmake)
include(cmake/LinkOptions.cmake)

add_subdirectory(src)
