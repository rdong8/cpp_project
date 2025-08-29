# Clangd Issue 860 Reproduction

## Dependencies

```bash
git clone -b clangd-issue-860 https://github.com/rdong8/cpp_project.git
cd cpp_project/
```

You need the following installed:

- Python with pip
- Clang 20+ (you may need to change the `CLANG_MAJOR_VERSION` variable below)
- Relatively recent CMake and Ninja, shouldn't be an issue

## Issue Reproduction

```bash
CONFIG=Debug
# You may need to change this
CLANG_MAJOR_VERSION=20
export BUILD_DIR=build
export CC=clang
export CXX=clang++

# Create Python virtual environment
mkdir -p .venv
python -m venv .venv
source .venv/bin/activate

# Set up Conan profile (only needs to be run once)
pip install conan
mkdir -p ~/.conan2/profiles
cat << EOF > $(conan config home)/profiles/default
[buildenv]
CC=clang
CXX=clang++

[conf]
tools.cmake.cmaketoolchain:generator=Ninja Multi-Config

[settings]
arch=x86_64
build_type=Release
compiler=clang
compiler.cppstd=23
compiler.libcxx=libc++
compiler.version=${CLANG_MAJOR_VERSION}
os=Linux
EOF

# Install spdlog
conan \
    install \
    -b missing \
    -pr:a default \
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

Then restart clangd in VS Code, go into [main.cpp](src/main.cpp) and then CTRL + click on the include ([`<spdlog/spdlog.h>`](src/main.cpp#L1)). Clangd should then crash.
