cmake_minimum_required(VERSION 4.3 FATAL_ERROR)

set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
# TODO: why is this broken again https://gitlab.kitware.com/cmake/cmake/-/work_items/27728
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(cpp_project
    VERSION 0.0.1
    DESCRIPTION "Template for a modern C++ project using CMake."
    HOMEPAGE_URL "https://github.com/rdong8/cpp_project"
    LANGUAGES
        C
        CXX
)

if (NOT CMAKE_CXX_COMPILER_ID MATCHES "^(Clang|GNU)$")
    message(FATAL_ERROR "Invalid compiler. Must be either Clang or GCC. Found: ${CMAKE_CXX_COMPILER_ID}")
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

include(cmake/Modules.cmake)

add_subdirectory(src)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if (PROJECT_IS_TOP_LEVEL)
    include(cmake/ClangTidy.cmake)

    # Needs to be done in the main CMakeLists since it calls enable_testing, which must be in the main CMakeLists
    include(CTest)
    add_subdirectory(tests)

    find_package(Doxygen
        CONFIG
        COMPONENTS
            dot
    )

    if (Doxygen_FOUND)
        add_subdirectory(docs)
    else ()
        message(STATUS "Doxygen not found, not building docs")
    endif ()
endif ()
