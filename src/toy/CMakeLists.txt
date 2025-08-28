
include(LLVM.cmake)

add_executable(toyc)
target_sources(toyc
        PUBLIC FILE_SET CXX_MODULES FILES
        toyc.cppm
)
# Run `llvm-config --components` to see all available components
llvm_map_components_to_libnames(llvm_libs support)
target_link_libraries(toyc
    PRIVATE 
    config
    llvm_config
    ${llvm_libs}
)
