cmake_minimum_required(VERSION 3.30.0 FATAL_ERROR)

if ("$<NOT:$<CXX_COMPILER_ID:Clang,GCC>>")
    message(FATAL_ERROR "Invalid compiler. Must be either Clang or GCC.")
endif ()

set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23) # TODO: Boost Conan package doesn't support C++26/CMAKE_CXX_STANDARD_LATEST
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(cpp_project
        VERSION 0.0.1
        DESCRIPTION "Template for a modern C++ project using CMake."
        HOMEPAGE_URL "https://github.com/rdong8/cpp_project"
        LANGUAGES CXX
)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if (PROJECT_IS_TOP_LEVEL)
    include(cmake/ClangTidy.cmake)

    # Generate compile_commands.json to make it easier to work with clang based tools
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    # Needs to be done in the main CMakeLists since it calls enable_testing, which must be in the main CMakeLists
    include(CTest)
    add_subdirectory(tests)

    find_package(Doxygen COMPONENTS dot)

    if (Doxygen_FOUND)
        add_subdirectory(docs)
    else ()
        message(STATUS "Doxygen not found, not building docs")
    endif ()
endif ()

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
