set dotenv-load

project := "cpp_project"

# Set to any non-empty string for extra output
verbose := ""

# Set a path to the compiler executables
# Will use the compiler's linker automatically (LLD for Clang, LD for GCC)
# Will add debug info on certain build types for the compiler's debugger automatically (LLDB for Clang, GDB for GCC)
export CXX := "clang++"

build_dir := "build"

# CMake supports Debug, Release, RelWithDebInfo, MinSizeRel to build *your code*
# 	Source: https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html
build_type := "Debug"

# Conan supports Debug, Release to build *your dependencies*
# 	Source: https://docs.conan.io/2/tutorial/consuming_packages/different_configurations.html
conan_build_type := "Release"

# The target to build and/or run
# 	Default build target is the default CMake target `all`
# 	Default run target is the target with the same name as the project, if present
target := "all"

# Default to provide to the executable
default_args := ""

system-deps:
	sudo dnf install llvm compiler-rt doxygen ninja mold
	sudo snap install cmake --classic

py-deps:
	pyenv virtualenv 3.12.5 {{ project }}
	pyenv activate {{ project }}
	pip install -r requirements.txt

# This only creates the profile, you still need to edit it to contain the details for your compiler and language
conan-profile:
	conan profile detect --force

conan-deps:
	BUILD_DIR={{ build_dir }} \
		conan \
			install . \
			-b missing \
			-s build_type={{ conan_build_type }} \
			-s "&:build_type={{ build_type }}"

cmake-config:
	cmake \
		-S . \
		-B {{ build_dir }} \
		-G "Ninja Multi-Config" \
		-DCMAKE_CXX_COMPILER={{ CXX }} \
		-DCMAKE_MAKE_PROGRAM=ninja \
		-DCMAKE_TOOLCHAIN_FILE={{ build_dir }}/{{ build_type }}/generators/conan_toolchain.cmake

build:
	cmake \
		--build {{ build_dir }} \
		--config {{ build_type }} \
		-t {{ target }} \
		-j \
		{{ if verbose != "" { "-v" } else { "" } }}

run *args=default_args:
	./{{ build_dir }}/src/{{ build_type }}/{{ if target == "all" { project } else { target } }} {{ args }}

test:
	ctest \
		--test-dir {{ build_dir }} \
		--extra-verbose \
		-C {{ build_type }} \
		-j

pre-commit:
	pre-commit install

clean:
	rm -rf {{ build_dir }}
