find_package(Boost REQUIRED)

add_library(mathematics)
target_sources(mathematics
        PUBLIC FILE_SET CXX_MODULES FILES
        mathematics.ccm
)
target_link_libraries(mathematics PRIVATE my_config)

add_executable(cpp_project)
target_sources(cpp_project
        PRIVATE FILE_SET CXX_MODULES FILES
        main.ccm
)
target_link_libraries(cpp_project
        PRIVATE
        my_config
        Boost::system
        Boost::coroutine
        Boost::regex
        Boost::thread
        Boost::date_time
        Boost::serialization
        mathematics
)
