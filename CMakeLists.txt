cmake_minimum_required(VERSION 3.30.0 FATAL_ERROR)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23) # TODO: Boost Conan package doesn't support C++26/CMAKE_CXX_STANDARD_LATEST
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_COLOR_DIAGNOSTICS ON)


project(cpp_project
        VERSION 0.0.1
        DESCRIPTION "Template for a modern C++ project using CMake."
        HOMEPAGE_URL "https://github.com/rdong8/cpp_project"
        LANGUAGES CXX
)

if ("$<NOT:$<CXX_COMPILER_ID:Clang,GCC>>")
    message(FATAL_ERROR "Invalid compiler. Must be either Clang or GCC.")
endif ()

# Only do these if this is the main project, and not if it is included through add_subdirectory
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # TODO: Currently broken
    # find_program(CLANG_TIDY NAMES clang-tidy REQUIRED)

    # set(CMAKE_CXX_CLANG_TIDY
    #     ${CLANG_TIDY}
    #     "-checks=-*,cppcoreguidelines-*"
    # )

    # Generate compile_commands.json to make it easier to work with clang based tools
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    # Nicely support folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Testing only available if this is the main app
    # Needs to be done in the main CMakeLists since it calls enable_testing, which must be in the main CMakeLists
    include(CTest)

    # Make test target depend on all (allows you to run tests without manually building first)
    set(CMAKE_SKIP_TEST_ALL_DEPENDENCY FALSE)

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

# Define compile options interface target
add_library(my_compile_options INTERFACE)
target_compile_features(my_compile_options INTERFACE "cxx_std_${CMAKE_CXX_STANDARD}")
include(cmake/CompileOptions.cmake)
set_project_compile_options(my_compile_options)

# Define compile warnings interface target
add_library(my_compile_warnings INTERFACE)
include(cmake/CompileWarnings.cmake)
set_project_warnings(my_compile_warnings)

# Define link options interface target
add_library(my_link_options INTERFACE)
include(cmake/LinkOptions.cmake)
set_project_link_options(my_link_options)

# Define sanitizers interface target and DEFAULT_SANITIZERS
add_library(my_sanitizers INTERFACE)
include(cmake/Sanitizers.cmake)
set_project_sanitizers(my_sanitizers)

# Define standard library interface target
add_library(my_stdlib INTERFACE)
include(cmake/StandardLibrary.cmake)
set_project_stdlib(my_stdlib)

# Define config interface target
add_library(config INTERFACE)
target_link_libraries(config
        INTERFACE
        my_compile_options
        my_compile_warnings
        my_link_options
        my_sanitizers
        my_stdlib
)

# Set IPO
include(cmake/InterproceduralOptimization.cmake)

add_subdirectory(src)
