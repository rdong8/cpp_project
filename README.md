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

In the [devcontainer configuration](.devcontainer/devcontainer.json), a volume has been configured for Conan's cache and configurations so this step only needs to be run once per devcontainer host.

Check if you already have a Conan profile with `uv run conan profile list`. If you haven't already created them, this command will create a default host and build profile for you:

```bash
just conan-profiles
```

Then, edit the host profile with `just edit-conan-profile host`. For example:

```toml
[buildenv]
CC=clang
CXX=clang++
# TODO: "Mold can only be built with libstdc++11", using LLD for now
# TODO: https://github.com/conan-io/conan/issues/15864
# TODO: https://github.com/conan-io/conan/issues/14174
# LD=mold

[conf]
# tools.build:exelinkflags=["-fuse-ld=mold"]
# tools.build:sharedlinkflags=["-fuse-ld=mold"]
tools.cmake.cmaketoolchain:generator=Ninja Multi-Config
# Like CC and CXX
tools.build:compiler_executables={"c": "clang", "cpp": "clang++"}

[platform_tool_requires]
# Tell Conan to look for CMake on the machine instead of installing it itself
cmake/4.1

[settings]
arch=x86_64
build_type=Release
compiler=clang
# TODO: 26 causing problems
compiler.cppstd=23
compiler.libcxx=libc++
compiler.version=20
os=Linux
```

And edit your build profile with `just edit-conan-profile build`. For example:

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
compiler.libcxx=libc++
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

At this point, you may want to restart clangd (`clangd: Restart language server` in VSCode command palette) so it picks up the new compile commands.

## Build

Build a target:

```bash
just build target-name
```

You can omit the target name to build everything.

The target will end up in `./build/src/path/to/target/build_type/target`.

## Docs

To open the documentation in the default browser (must be built first via `just build docs`):

```bash
just docs
```

You may also pass a command that will be used to open the `index.html` file:

```bash
just docs firefox
just docs 'flatpak run com.brave.Browser' # You need to use Flatseal to give the flatpak permission in this case
```

## Test

Run all tests:

```bash
just test
```

Any [flags](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Testing%20With%20CMake%20and%20CTest.html#testing-using-ctest) will be forwarded to `ctest`, for example:

```bash
just test -R math # Run tests matching regular expression "math"
```

## Pre-Commit

While developing, you may want to have some tasks automatically run with pre-commit.

```bash
just pre-commit
```

## Clean

Clean the build directory and Conan generated files:

```bash
just clean
```

Clean Python files as well:

```bash
just clean-all
```

You'll need to make the project's Conan dependencies and run the CMake config again with `just conan-deps config`.

You can also delete all installed Conan packages matching a pattern:

```bash
just clean-conan 'boost/*'
just clean-conan # Removes everything
```
