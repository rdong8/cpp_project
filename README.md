# cpp_project
Template for a modern C++ project using CMake.

## Dependencies

Ensure you have the LLVM toolchain installed, including `compiler-rt` for the sanitizers to work.

## Clone
```bash
git clone --recurse-submodules https://github.com/rdong8/cpp_project.git
```

## Configure

```bash
cd cpp_project/
CXX=$(which clang++) cmake -S . -B build/
```

## Build

```bash
cmake --build build/
```

The `cpp_project` executable will be in `build/src/Debug`.

## Adding a Submodule Dependency

```bash
cd external/
git submodule add https://github.com/gabime/spdlog.git
```

Then in your CMakeLists.txt file:
```cmake
add_subdirectory(external/spdlog)
if(NOT TARGET spdlog)
    find_package(spdlog REQUIRED)
endif()

# To use:
target_link_libraries(my_target PRIVATE spdlog::spdlog)
```