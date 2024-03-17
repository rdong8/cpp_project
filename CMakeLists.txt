cmake_minimum_required(VERSION 3.28)

project(cpp_project
        VERSION 0.0.1
        DESCRIPTION "Template for a modern C++ project using CMake."
        HOMEPAGE_URL "https://github.com/rdong8/cpp_project"
        LANGUAGES CXX
)

# Only these 2 compilers are supported for now
# https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ID.html#variable:CMAKE_%3CLANG%3E_COMPILER_ID
if ("$<NOT:$<CXX_COMPILER_ID:Clang,GCC>>")
    message(FATAL_ERROR "Invalid compiler. Must be either Clang or GCC.")
endif ()

# Only do these if this is the main project, and not if it is included through add_subdirectory
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    # Optionally set things like CMAKE_CXX_STANDARD,
    # CMAKE_POSITION_INDEPENDENT_CODE here
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED True)

    # Ensure -std=c++xx instead of -std=g++xx
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Generate compile_commands.json to make it easier to work with clang based tools
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    # Nicely support folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Testing only available if this is the main app
    # Needs to be done in the main CMakeLists since it calls enable_testing, which must be in the main CMakeLists
    include(CTest)

    # Docs only available if this is the main app
    find_package(Doxygen COMPONENTS dot)
    if (Doxygen_FOUND)
        add_subdirectory(docs)
    else ()
        message(STATUS "Doxygen not found, not building docs")
    endif ()
endif ()

# Testing only available if this is the main app
# Emergency override BUILD_TESTING provided as well
if ((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR BUILD_TESTING)
        AND BUILD_TESTING)
    add_subdirectory(tests)
endif ()

# Logging
find_package(spdlog REQUIRED)

# Define compile warnings interface target
add_library(my_warnings INTERFACE)
include(cmake/CompileWarnings.cmake)
set_project_warnings(my_warnings)

# Define sanitizers interface target and DEFAULT_SANITIZERS
add_library(my_sanitizers INTERFACE)
include(cmake/Sanitizers.cmake)
set_project_sanitizers(my_sanitizers)

# Define compile options interface target
add_library(my_compile_options INTERFACE)
target_compile_features(my_compile_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})
include(cmake/CompileOptions.cmake)
set_project_compile_options(my_compile_options)

# Define standard library interface target
add_library(my_stdlib INTERFACE)
include(cmake/StandardLibrary.cmake)
set_project_stdlib(my_stdlib)

# Define link options interface target
add_library(my_link_options INTERFACE)
include(cmake/LinkOptions.cmake)
set_project_link_options(my_compile_options)

# Define config interface target
add_library(my_config INTERFACE)
target_link_libraries(my_config
        INTERFACE
        my_compile_options
        my_link_options
        my_sanitizers
        my_warnings
        spdlog::spdlog
)

# Set IPO
include(cmake/InterproceduralOptimization.cmake)

add_subdirectory(src)
