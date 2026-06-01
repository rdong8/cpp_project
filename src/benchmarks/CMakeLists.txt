find_package(Boost
    REQUIRED
    CONFIG
    COMPONENTS
        container
)

find_package(LLVM REQUIRED CONFIG)
find_package(absl REQUIRED CONFIG)
find_package(benchmark REQUIRED CONFIG)

add_module_executable(benchmarks.hash_maps hash_maps)
target_link_libraries(benchmarks.hash_maps
    PRIVATE
        Boost::container
        abseil::abseil
        benchmark::benchmark_main
)
llvm_map_components_to_libnames(LLVM_LIBS support)
target_link_libraries(benchmarks.hash_maps
    PRIVATE
        "${LLVM_LIBS}"
)
target_include_directories(benchmarks.hash_maps
    SYSTEM PRIVATE
        "${LLVM_INCLUDE_DIRS}"
)