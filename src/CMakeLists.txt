find_package(Boost REQUIRED)

add_executable(cpp_project)
target_sources(cpp_project
        PRIVATE
        main.cpp
)
target_link_libraries(cpp_project
        PRIVATE
        config
        Boost::headers
)
