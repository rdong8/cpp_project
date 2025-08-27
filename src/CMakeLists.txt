find_package(spdlog REQUIRED)

add_library(my_module)
target_sources(my_module
        PUBLIC FILE_SET CXX_MODULES FILES
        my_module.cppm
)
target_link_libraries(my_module
        PRIVATE
        config
        spdlog::spdlog
)

add_executable(main)
target_sources(main
        PUBLIC
        main.cpp
)
target_link_libraries(main
        PRIVATE
        config
        my_module
)
