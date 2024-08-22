find_package(Catch2 REQUIRED)

add_executable(test_math)
target_sources(test_math PRIVATE mathematics.cc)
target_link_libraries(test_math
        PRIVATE
        config
        Catch2::Catch2WithMain
        mathematics
)
add_test(NAME test_math COMMAND test_math)
