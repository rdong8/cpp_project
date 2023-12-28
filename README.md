# cpp_project
Template for a modern C++ project using CMake.

## Clone
```bash
git clone --recurse-submodules https://github.com/rdong8/cpp_project.git
cd cpp_project/
```

## Dependencies

Ensure you have the LLVM toolchain installed, including `compiler-rt` for the sanitizers to work. Also creates a `pyenv`
environment and installs the necessary packages.

```bash
make system-deps py-deps
```

### Conan

If you haven't already, create a conan profile.

```bash
make conan-profile
```

Then install the project's C++ dependencies with Conan.

```bash
make conan-deps
```

## Configure

Either run:

```bash
make cmake-config
```

or create a CMake profile in CLion with the following settings:

- Name: Debug
- Build type: Debug
- Toolchain: Use Default
- Generator: Ninja Multi-Config
- CMake options:
    ```
    -G "Ninja Multi-Config" -DCMAKE_TOOLCHAIN_FILE=build/Debug/generators/conan_toolchain.cmake
    ```
- Build directory: `build/`
- Build options: empty
- Environment: empty

## Build

```bash
make build
```

The `cpp_project` executable will be in `build/src/Debug/`.

## Pre-Commit

While developing, you may want to have some tasks automatically run with pre-commit.

```bash
make pre-commit
```
