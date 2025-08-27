# cpp_project

Template for a modern C++ project using CMake.

## Notes

- Only Linux is supported
- This project uses [devcontainers](https://containers.dev/) to avoid dependencies polluting the host. Specifically, we use [podman](https://podman.io/) instead of Docker, which enables the devcontainer to run on a host that is itself containerized (ie. a [GCP Cloud Workstation](https://cloud.google.com/workstations?hl=en) instance)
- Because containers are rootless, most dev tools are installed with `brew` instead

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

Open the devcontainer in your IDE (CTRL + Shift + P in VS Code, then "Dev Containers: Rebuild and Reopen in Container").

All commands after this point are to be run *in the devcontainer*, not on the host.

## Issue Reproduction

```bash
cd /workspaces/cpp_project

CONFIG=Debug
export BUILD_DIR=build
export CC=clang
export CXX=clang++

# Set up Conan
uv venv
uv pip install conan

cat << EOF > $(uv run conan config home)/profiles/host
[buildenv]
CC=clang
CXX=clang++
LD=mold

[conf]
tools.build:exelinkflags=["-fuse-ld=mold"]
tools.build:sharedlinkflags=["-fuse-ld=mold"]
tools.cmake.cmaketoolchain:generator=Ninja Multi-Config

[settings]
arch=x86_64
build_type=${CONFIG}
compiler=clang
compiler.cppstd=23
compiler.libcxx=libstdc++11
compiler.version=20
os=Linux
EOF

cat << EOF > $(uv run conan config home)/profiles/build
[conf]
tools.cmake.cmaketoolchain:generator=Ninja Multi-Config

[settings]
arch=x86_64
build_type=${CONFIG}
compiler=clang
compiler.cppstd=23
compiler.libcxx=libstdc++11
compiler.version=20
os=Linux
EOF

# Install mold
uv run conan \
    install \
    -b missing \
    -pr:b build \
    -pr:h host \
    -s '&':build_type=${CONFIG} \
    --requires='mold/[*]'

# Install other dependencies
uv run conan \
    install \
    -b missing \
    -pr:b build \
    -pr:h host \
    -s '&':build_type=${CONFIG} \
    .

# CMake configure
cmake \
    -S . \
    -B ${BUILD_DIR} \
    --preset conan-default

# CMake build
cmake \
    --build ${BUILD_DIR} \
    --preset conan-debug \
    --target main

# Run
./${BUILD_DIR}/src/${CONFIG}/main
```
