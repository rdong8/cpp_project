# cpp_project
Template for a modern C++ project using CMake.

## Clone
```bash
git clone --recurse-submodules https://github.com/rdong8/cpp_project.git
cd cpp_project/
```

## Dependencies

### LLVM

Ensure you have the LLVM toolchain installed, including `compiler-rt` for the sanitizers to work.

```bash
sudo dnf install llvm compiler-rt
```

### Conan

Install conan, preferably through pip with pyenv.

```bash
pyenv virtualenv 3.12.2 cpp_project
pyenv local cpp_project
pip install conan
```

If you haven't already, create a conan profile.

```bash
conan profile detect --force
```

### Install Dependencies

```bash
conan install . --build=missing --settings=build_type=Debug
```

## Configure

```bash
cmake -S . -B build/ -G "Ninja Multi-Config" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_MAKE_PROGRAM=Ninja \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_TOOLCHAIN_FILE=build/Debug/generators/conan_toolchain.cmake
```

## Build

```bash
cmake --build build/
```

The `cpp_project` executable will be in `build/src/Debug/`.
