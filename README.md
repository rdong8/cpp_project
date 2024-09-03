# cpp_project

Template for a modern C++ project using CMake.

Ideally you should read through both the README and the `justfile` to better understand how everything works.

Note: All `justfile` commands should be run under a shell in which your `.bashrc`, `.zshrc` or equivalent has been
sourced. Do not use the run button in your IDE, `pyenv` will likely not work.

## Clone

```bash
git clone --recurse-submodules https://github.com/rdong8/cpp_project.git
cd cpp_project/
```

## Dependencies

### System

```bash
just system-deps
```

### Python

First ensure you have [pyenv](https://github.com/pyenv/pyenv) and
[pyenv-virtualenv](https://github.com/pyenv/pyenv-virtualenv) installed. Then:

```bash
just py-deps
```

### Conan

#### Profile

Check if you already have a Conan profile with `conan profile list`. If you haven't already made one, create one:

```bash
just conan-profile
```

Then, modify the `$(conan config home)/profiles/default` profile generated to add a `[buildenv]` section, and put in
your compiler and language details:

```toml
[buildenv]
CC=clang
CXX=clang++

[settings]
arch=x86_64
build_type=Release
compiler=clang
compiler.cppstd=23
compiler.libcxx=libc++
compiler.version=18
os=Linux
```

Note that the build type here is for your dependencies, which you can compile in release mode even if you are building
your own code in debug. A variable `CONAN_BUILD_TYPE` is provided in the Makefile to override the build type.

#### Build Dependencies

Now build the project's C++ dependencies with Conan:

```bash
just conan-deps
```

## Configure

First, go in the `justfile` and set the paths to the C and C++ compilers.

Then either run:

```bash
just cmake-config
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

To build the default target:

```bash
just build
```

To build a specific target:

```bash
just target=docs build
```

## Run

Either set the `target` variable in the Makefile or do it on the command line:

```bash
just target=main run
```

If `target` is not set, it will try to run the target with the same name as the project.

You can also provide arguments to the executable by adding them after the `run` command:

```bash
just run arg1 arg2 arg3
```

## Test

```bash
just test
```

## Pre-Commit

While developing, you may want to have some tasks automatically run with pre-commit.

```bash
just pre-commit
```

## Clean

Cleans the build directory.

```bash
just clean
```

You'll need to make the project's Conan dependencies and run the CMake config again with `just conan-deps cmake-config`.
