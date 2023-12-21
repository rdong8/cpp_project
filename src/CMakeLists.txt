add_executable(cpp_project main.cc)
target_link_libraries(cpp_project PRIVATE my_config)
target_include_directories(cpp_project PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
