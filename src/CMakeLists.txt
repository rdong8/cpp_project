find_package(Boost REQUIRED)

add_executable(cpp_project main.cc)
target_link_libraries(cpp_project PRIVATE
        my_config
        Boost::system
        Boost::coroutine
        Boost::regex
        Boost::thread
        Boost::date_time
        Boost::serialization
)
target_include_directories(cpp_project PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
