find_package(Boost REQUIRED CONFIG)
find_package(spdlog REQUIRED CONFIG)

add_library(mathematics)
target_sources(mathematics
    PUBLIC FILE_SET CXX_MODULES FILES
        mathematics.cppm
)
target_link_libraries(mathematics PRIVATE config)

add_executable(library_usage)
target_sources(library_usage PUBLIC main.cpp)
target_link_libraries(library_usage
    PRIVATE
        config
        mathematics
        Boost::headers
        Boost::cobalt
        spdlog::spdlog
)

# Install Common Package Specification (CPS) package description
# https://cmake.org/cmake/help/latest/command/install.html#package-info
install(
    PACKAGE_INFO Mathematics
    EXPORT mathematics
    VERSION 0.0.1
)
