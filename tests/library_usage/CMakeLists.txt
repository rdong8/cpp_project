add_executable(test_math)
target_sources(test_math
        PRIVATE FILE_SET CXX_MODULES FILES
        mathematics.cppm
)
target_link_libraries(test_math
        PRIVATE
        config
        Catch2::Catch2WithMain
        mathematics
)
add_test(NAME test_math COMMAND test_math)
