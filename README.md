# cpp_project

Template for a modern C++ project using CMake.

Read through both the README and the `justfile` to better understand how everything works.

## Notes

- Only Linux is supported
- This project uses [devcontainers](https://containers.dev/) to avoid dependencies polluting the host. Specifically, we use [podman](https://podman.io/) instead of Docker, which enables the devcontainer to run on a host that is itself containerized (ie. a [GCP Cloud Workstation](https://cloud.google.com/workstations?hl=en) instance)
- Because containers are rootless, most dev tools are installed with `brew` instead
- The devcontainer is setup to use [fish](https://fishshell.com/) as the default shell

## Initialize

*On the host*:

```bash
git clone --recurse-submodules https://github.com/rdong8/cpp_project.git
cd cpp_project/
```

Also make sure you have [`podman`](https://podman.io/docs/installation#installing-on-linux) installed on the host. For example:

```bash
# Fedora
sudo dnf -y install podman

# Ubuntu
sudo apt update
sudo apt -y install podman
```

All commands after this point are to be run *in the devcontainer*, not on the host.

## Dependencies

### Python

First ensure you have [uv](https://github.com/astral-sh/uv) installed. Then:

```bash
just venv # Creates a virtual environment
just py-deps # Installs the Python dependencies. Use `just py-deps 1` to force a reinstall.
```

### Conan

#### Profile

Check if you already have a Conan profile with `uv run conan profile list` (you shouldn't if you set up the project with the devcontainer). If you haven't already created them, this command will create a default host and build profile for you:

```bash
just conan-profiles
```

Then, edit the host profile with `just edit-conan-profile host`.

```toml
[buildenv]
CC=clang
CXX=clang++
LD=mold

[conf]
tools.build:exelinkflags=["-fuse-ld=mold"]
tools.build:sharedlinkflags=["-fuse-ld=mold"]
tools.cmake.cmaketoolchain:generator=Ninja Multi-Config
# Paths to your compilers if they're not `clang` and `clang++`
# tools.build:compiler_executables={"c": "clang-20", "cpp": "clang++-20"}

[platform_tool_requires]
# Tell Conan to look for CMake on the machine instead of installing it itself
cmake/4.1

[settings]
arch=x86_64
build_type=Release
compiler=clang
# TODO: 26 causing problems
compiler.cppstd=23
# TODO: "Mold can only be built with libstdc++11", can't use libc++
compiler.libcxx=libstdc++11
compiler.version=20
os=Linux
```

And edit your build profile with `just edit-conan-profile build`:

```toml
[conf]
tools.cmake.cmaketoolchain:generator=Ninja Multi-Config

[platform_tool_requires]
cmake/4.1

[settings]
arch=x86_64
build_type=Release
compiler=clang
compiler.cppstd=23
compiler.libcxx=libstdc++11
compiler.version=20
os=Linux
```

Note that the build type here is for *your dependencies*, which you can compile in release mode even if you are building your own code in debug.

The build type for your own code is controlled by the `build` variable in the justfile.

#### Build Dependencies

Now build the project's C++ dependencies with Conan:

```bash
just conan-install-all
```

## Configure

This command runs the CMake configure. Theoretically it only needs to be run once, since the `just build` command invokes `cmake --build` which should automatically reconfigure if necessary. In practice if you're having CMake issues, just try rerunning this.

```bash
just config
```

## Build

To build the default target with the default arguments specified in the `justfile`:

```bash
just build
```

To build a specific target:

```bash
just build docs
```

## Run

To run the default target with the default arguments specified in the `justfile`:

```bash
just run
```

To run a specific target:

```bash
just run cpp_project
```

To run a target with arguments:

```bash
just run cpp_project arg1 arg2 arg3
```

## Docs

To open the documentation (must be built first via `just build docs`):

```bash
just docs
```

To open with a particular browser, pass the path to the command that will be passed the `index.html` file of the browser:

```bash
just docs firefox
just docs "flatpak run com.brave.Browser" # You need to use Flatseal to give the flatpak permission in this case
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

You'll need to make the project's Conan dependencies and run the CMake config again with `just conan-deps config`.
