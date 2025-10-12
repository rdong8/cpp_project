# References:
# https://llvm.org/docs/CMake.html#embedding-llvm-in-your-project
# https://github.com/llvm/llvm-project/blob/main/mlir/examples/standalone/CMakeLists.txt

add_library(llvm_config INTERFACE)

find_package(MLIR REQUIRED CONFIG)

set(LLVM_RUNTIME_OUTPUT_INTDIR "${CMAKE_BINARY_DIR}/bin")
set(LLVM_LIBRARY_OUTPUT_INTDIR "${CMAKE_BINARY_DIR}/lib")
set(MLIR_BINARY_DIR ${CMAKE_BINARY_DIR})

list(APPEND CMAKE_MODULE_PATH "${MLIR_CMAKE_DIR}")
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")

include(TableGen)
include(AddLLVM)
include(AddMLIR)
include(HandleLLVMOptions)

# Run `llvm-config --components` to see all available components
llvm_map_components_to_libnames(LLVM_LIBS support)
target_link_libraries(llvm_config
    INTERFACE
    "${LLVM_LIBS}"
    MLIRSupport
)

target_include_directories(llvm_config
    INTERFACE
    "${LLVM_INCLUDE_DIRS}"
    "${MLIR_INCLUDE_DIRS}"
)
target_link_directories(llvm_config
    INTERFACE
    "${LLVM_BUILD_LIBRARY_DIR}"
)
target_compile_definitions(llvm_config
    INTERFACE
    "${LLVM_DEFINITIONS}"
)
