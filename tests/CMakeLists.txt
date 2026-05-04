find_package(Catch2 REQUIRED CONFIG)

# Pass it the name of an existing module library target
# TODO: https://chuanqixu9.github.io/c++/2025/12/30/C++20-Modules-Best-Practices.en.html#use-module-implementation-partition-units-to-write-unit-tests
function(add_module_test name subpath)
    set(TEST_NAME "test_${name}")
    add_module_executable("${TEST_NAME}" "${subpath}")

    target_link_libraries("${TEST_NAME}"
        PRIVATE
            Catch2::Catch2WithMain
            config
            "${name}"
    )

    add_test(NAME "${TEST_NAME}" COMMAND "${TEST_NAME}")
endfunction()

add_subdirectory(library_usage)
