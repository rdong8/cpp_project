# cpp_project

Template for a modern C++ project using CMake.

Ideally you should read through both the README and the Makefile to better understand how everything works.

Note: All Makefile commands should be run under a shell in which your `.bashrc`, `.zshrc` or equivalent has been
sourced. Do not use the run button in your IDE's markdown preview or Makefile editor, `pyenv` will likely not work.

## Clone

```bash
git clone --recurse-submodules https://github.com/rdong8/cpp_project.git
cd cpp_project/
```

## Dependencies

### System

```bash
make system-deps
```

### Python

First ensure you have [pyenv](https://github.com/pyenv/pyenv) and
[pyenv-virtualenv](https://github.com/pyenv/pyenv-virtualenv) installed. Then:

```bash
make py-deps
```

### Conan

#### Profile

Check if you already have a Conan profile with `conan profile list`. If you haven't already made one, create one:

```bash
make conan-profile
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
compiler.version=17
os=Linux
```

Note that the build type here is for your dependencies, which you can compile in release mode even if you are building
your own code in debug. A variable `CONAN_BUILD_TYPE` is provided in the Makefile to override the build type.

#### Build Dependencies

Now build the project's C++ dependencies with Conan:

```bash
make conan-deps
```

## Configure

First, go in the Makefile and set the paths to the C and C++ compilers.

Then either run:

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

To build the default target:

```bash
make build
```

To build a specific target:

```bash
make build TARGET=docs
```

## Run

Either set the `TARGET` variable in the Makefile or do it on the command line:

```bash
make run TARGET=main
```

If `TARGET` is not set, it will try to run the target with the same name as the project.

You can also provide arguments to the executable:

```bash
make run ARGS="arg1 arg2 arg3"
```

Make sure to quote them so that they're interpreted as a single string assigned to `ARGS`.

## Test

```bash
make test
```

## Pre-Commit

While developing, you may want to have some tasks automatically run with pre-commit.

```bash
make pre-commit
```

## Clean

Cleans the build directory.

```bash
make clean
```

You'll need to make the project's Conan dependencies again with `make conan-deps`, and then
configure CMake with `make cmake-config`.
