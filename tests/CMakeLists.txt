find_package(Catch2 REQUIRED CONFIG)

add_library(tests.config INTERFACE)
target_sources(tests.config
    INTERFACE FILE_SET HEADERS FILES
        test_main.hpp
)
target_link_libraries(tests.config
    INTERFACE
        Catch2::Catch2
        config
)

# Pass it the name of an existing module library target
# Write your test as a module implementation unit of the module being tested and use `TEST_MAIN()` to generate the main function
# This gives you access to the module internals
# https://chuanqixu9.github.io/c++/2025/12/30/C++20-Modules-Best-Practices.en.html#use-module-implementation-partition-units-to-write-unit-tests
function(add_module_test name subpath)
    set(TEST_NAME "test_${name}")
    add_module_executable("${TEST_NAME}" "${subpath}")

    target_link_libraries("${TEST_NAME}"
        PRIVATE
            tests.config
            "${name}"
    )

    add_test(NAME "${TEST_NAME}" COMMAND "${TEST_NAME}")
endfunction()

add_subdirectory(library_usage)
