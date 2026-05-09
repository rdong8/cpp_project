find_package(Boost
    REQUIRED
    CONFIG
    COMPONENTS
        cobalt
)

find_package(spdlog REQUIRED CONFIG)

add_module_library(library_usage.mathematics mathematics)

add_module_executable(library_usage .)
target_link_libraries(library_usage
    PRIVATE
        Boost::cobalt
        spdlog::spdlog
        library_usage.mathematics
)
