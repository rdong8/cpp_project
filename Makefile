PROJECT = cpp_project

# Set a path to the compiler executables
# It's set to use the compiler's linker automatically (LLD for Clang, LD for GCC)
CC = clang
CXX = clang++

BUILD_DIR = build
BUILD_TYPE = Debug

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

.PHONY: conan-profile
conan-profile:
	conan profile detect --force

.PHONY: conan-deps
conan-deps:
	conan install . --build=missing --settings=build_type=${BUILD_TYPE}

.PHONY: cmake-config
cmake-config:
	cmake -S . -B ${BUILD_DIR} -G "Ninja Multi-Config" \
	  -DCMAKE_MAKE_PROGRAM=ninja \
	  -DCMAKE_C_COMPILER=${CC} \
	  -DCMAKE_CXX_COMPILER=${CXX} \
	  -DCMAKE_TOOLCHAIN_FILE=${BUILD_DIR}/${BUILD_TYPE}/generators/conan_toolchain.cmake

.PHONY: build
build:
	cmake --build ${BUILD_DIR} --target $(if $(TARGET),$(TARGET),all) -j

.PHONY: run
run:
	./${BUILD_DIR}/src/${BUILD_TYPE}/$(if $(TARGET),$(TARGET),$(PROJECT)) ${ARGS}

.PHONY: test
test:
	ctest --test-dir ${BUILD_DIR} --extra-verbose -C ${BUILD_TYPE}

.PHONY: pre-commit
pre-commit:
	pre-commit install

.PHONY: clean
clean:
	rm -rf ${BUILD_DIR}
