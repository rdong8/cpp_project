add_module_executable(test_math mathematics)
target_link_libraries(test_math
    PRIVATE
        Catch2::Catch2WithMain
        mathematics
)
add_test(NAME test_math COMMAND test_math)
