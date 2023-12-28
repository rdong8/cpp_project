PROJECT = cpp_project

CC = clang
CXX = clang++

BUILD_DIR = build/
BUILD_TYPE = Debug

.PHONY: system-deps
system-deps:
	sudo dnf install llvm compiler-rt cmake

.PHONY:
py-deps:
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
	  -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
	  -DCMAKE_MAKE_PROGRAM=ninja \
	  -DCMAKE_C_COMPILER=${CC} \
	  -DCMAKE_CXX_COMPILER=${CXX} \
	  -DCMAKE_TOOLCHAIN_FILE=${BUILD_DIR}${BUILD_TYPE}/generators/conan_toolchain.cmake

.PHONY: build
build:
	cmake --build ${BUILD_DIR}

.PHONY: pre-commit
pre-commit:
	pre-commit install
