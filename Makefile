PROJECT = cpp_project

# Set a path to the compiler executables
# Will use the compiler's linker automatically (LLD for Clang, LD for GCC)
# Will add debug info on certain build types for the compiler's debugger automatically (LLDB for Clang, GDB for GCC)
CXX = clang++

BUILD_DIR = build

# CMake supports Debug, Release, RelWithDebInfo, MinSizeRel to build *your code*
# 	Source: https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html
BUILD_TYPE = Debug

# Conan supports Debug, Release to build *your dependencies*
# 	Source: https://docs.conan.io/2/tutorial/consuming_packages/different_configurations.html
CONAN_BUILD_TYPE = Release

# The target to build and/or run
# 	Default build target is the default CMake target `all`
# 	Default run target is the target with the same name as the project, if present
TARGET =

# Arguments to provide to the executable in `make run`
ARGS =

.PHONY: system-deps
system-deps:
	sudo dnf install llvm compiler-rt cmake doxygen

.PHONY: py-deps
py-deps: requirements.txt
	pyenv virtualenv 3.12.1 ${PROJECT}
	pyenv local ${PROJECT}
	pip install -r requirements.txt

# This only creates the profile, you still need to edit it to contain the details for your compiler and language
.PHONY: conan-profile
conan-profile:
	conan profile detect --force

.PHONY: conan-deps
conan-deps:
	conan \
		install . \
		-b missing \
		-s build_type=${CONAN_BUILD_TYPE} \
		-s "&:build_type=${BUILD_TYPE}"

.PHONY: cmake-config
cmake-config:
	cmake \
		-S . \
		-B ${BUILD_DIR} \
		-G "Ninja Multi-Config" \
		-DCMAKE_CXX_COMPILER=${CXX} \
		-DCMAKE_MAKE_PROGRAM=ninja \
		-DCMAKE_TOOLCHAIN_FILE=${BUILD_DIR}/${BUILD_TYPE}/generators/conan_toolchain.cmake

.PHONY: build
build:
	cmake \
		--build ${BUILD_DIR} \
		--config ${BUILD_TYPE} \
		-t $(if $(TARGET),$(TARGET),all) \
		-j

.PHONY: run
run:
	./${BUILD_DIR}/src/${BUILD_TYPE}/$(if $(TARGET),$(TARGET),$(PROJECT)) ${ARGS}

.PHONY: test
test:
	ctest \
		--test-dir ${BUILD_DIR} \
		--extra-verbose \
		-C ${BUILD_TYPE} \
		-j

.PHONY: pre-commit
pre-commit:
	pre-commit install

.PHONY: clean
clean:
	rm -rf ${BUILD_DIR}
