find_package(spdlog REQUIRED)

add_executable(main)
target_sources(main
        PUBLIC
        main.cpp
)
target_link_libraries(main
        PRIVATE
        config
        spdlog::spdlog
)
