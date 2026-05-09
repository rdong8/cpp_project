# References:
# https://llvm.org/docs/CMake.html#embedding-llvm-in-your-project

add_library(llvm_config INTERFACE)

find_package(LLVM REQUIRED CONFIG)

set(LLVM_RUNTIME_OUTPUT_INTDIR "${CMAKE_BINARY_DIR}/bin")
set(LLVM_LIBRARY_OUTPUT_INTDIR "${CMAKE_BINARY_DIR}/lib")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")

include(AddLLVM)
include(HandleLLVMOptions)

# Run `llvm-config --components` to see all available components
llvm_map_components_to_libnames(LLVM_LIBS support)
target_link_libraries(llvm_config
    INTERFACE
        "${LLVM_LIBS}"
)

target_include_directories(llvm_config
    SYSTEM INTERFACE
        "${LLVM_INCLUDE_DIRS}"
)
target_link_directories(llvm_config
    INTERFACE
        "${LLVM_BUILD_LIBRARY_DIR}"
)
target_compile_definitions(llvm_config
    INTERFACE
        "${LLVM_DEFINITIONS}"
)
