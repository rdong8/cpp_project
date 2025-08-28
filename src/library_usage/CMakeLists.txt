find_package(Boost REQUIRED)
find_package(spdlog REQUIRED)

add_library(mathematics)
target_sources(mathematics
        PUBLIC FILE_SET CXX_MODULES FILES
        mathematics.cppm
)
target_link_libraries(mathematics PRIVATE config)

add_executable(library_usage)
target_sources(library_usage
        PUBLIC
        main.cpp
)
target_link_libraries(library_usage
        PRIVATE
        config
        mathematics
        Boost::headers
        Boost::cobalt
        spdlog::spdlog
)
