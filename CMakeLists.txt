cmake_minimum_required(VERSION 4.3 FATAL_ERROR)

set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

# C language needs to be enabled for LLVM
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
endif()

add_library(config INTERFACE)

target_compile_features(config INTERFACE "cxx_std_${CMAKE_CXX_STANDARD}")

include(cmake/CompileOptions.cmake)
set_compile_options(config)

include(cmake/CompileWarnings.cmake)
set_compile_warnings(config)

include(cmake/Sanitizers.cmake)
set_sanitizers(config)

include(cmake/InterproceduralOptimization.cmake)

include(cmake/Modules.cmake)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if (PROJECT_IS_TOP_LEVEL)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
    else()
        message(STATUS "Doxygen not found, not building docs")
    endif()
endif()

# Must happen after `CMAKE_EXPORT_COMPILE_COMMANDS` is set
add_subdirectory(src)
