cmake_minimum_required(VERSION 3.27.0)

project(
  cpp_project
  VERSION 0.0.1
  DESCRIPTION "Template for a modern C++ project using CMake."
  HOMEPAGE_URL "https://github.com/rdong8/cpp_project"
  LANGUAGES CXX)

# Only do these if this is the main project, and not if it is included through
# add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

  # Optionally set things like CMAKE_CXX_STANDARD,
  # CMAKE_POSITION_INDEPENDENT_CODE here
  set(CMAKE_CXX_STANDARD 23)
  set(CMAKE_CXX_STANDARD_REQUIRED True)

  # Let's ensure -std=c++xx instead of -std=g++xx
  set(CMAKE_CXX_EXTENSIONS OFF)

  # Set a default build type if none was specified
  if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE
        Debug
        CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui, ccmake
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
                                                 "MinSizeRel" "RelWithDebInfo")
  endif()

  # Generate compile_commands.json to make it easier to work with clang based
  # tools
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  # Let's nicely support folders in IDEs
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  # Testing only available if this is the main app Note this needs to be done in
  # the main CMakeLists since it calls enable_testing, which must be in the main
  # CMakeLists.
  include(CTest)

  # Docs only available if this is the main app
  find_package(Doxygen COMPONENTS dot)
  if(Doxygen_FOUND)
    add_subdirectory(docs)
  else()
    message(STATUS "Doxygen not found, not building docs")
  endif()
endif()

# Logging
find_package(spdlog REQUIRED)

# Define compile warnings interface target
add_library(my_warnings INTERFACE)
include(cmake/CompileWarnings.cmake)
my_set_project_warnings(my_warnings)

# Define compile options interface target
add_library(my_compile_options INTERFACE)
target_compile_features(my_compile_options
                        INTERFACE cxx_std_${CMAKE_CXX_STANDARD})
include(cmake/CompileOptions.cmake)
my_set_project_compile_options(my_compile_options)

# Define link options interface target
add_library(my_link_options INTERFACE)
include(cmake/LinkOptions.cmake)
my_set_project_link_options(my_compile_options)

# Define config interface target
add_library(my_config INTERFACE)
target_link_libraries(my_config INTERFACE my_compile_options my_link_options
                                          my_warnings spdlog::spdlog)

# The executable code is here
add_subdirectory(src)

# Testing only available if this is the main app Emergency override
# MODERN_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING)
   AND BUILD_TESTING)
  add_subdirectory(tests)
endif()
