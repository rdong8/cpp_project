find_package(Catch2 REQUIRED)

add_executable(test_math mathematics.cc)
target_link_libraries(test_math PRIVATE my_config Catch2::Catch2WithMain
                                        mathematics)
target_include_directories(test_math PRIVATE ${PROJECT_SOURCE_DIR})
add_test(NAME test_math COMMAND test_math)
