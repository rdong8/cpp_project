cmake_minimum_required(VERSION 4.3 FATAL_ERROR)

if ("$<NOT:$<CXX_COMPILER_ID:Clang,GCC>>")
    message(FATAL_ERROR "Invalid compiler. Must be either Clang or GCC.")
endif ()

set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
# https://gitlab.kitware.com/cmake/cmake/-/blob/master/Help/dev/experimental.rst?ref_type=heads
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
set(CMAKE_EXPERIMENTAL_EXPORT_BUILD_DATABASE "73194a1d-c0b5-41b9-9190-a4512925e192")
set(CMAKE_EXPORT_BUILD_DATABASE ON)

project(cpp_project
        VERSION 0.0.1
        DESCRIPTION "Template for a modern C++ project using CMake."
        HOMEPAGE_URL "https://github.com/rdong8/cpp_project"
        LANGUAGES C CXX
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
set_project_compile_warnings(config)

include(cmake/Sanitizers.cmake)
set_project_sanitizers(config)

include(cmake/InterproceduralOptimization.cmake)

add_subdirectory(src)
