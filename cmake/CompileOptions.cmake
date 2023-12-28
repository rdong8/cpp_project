# Enhance error reporting and compiler messages
if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    add_compile_options(-fcolor-diagnostics)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-fdiagnostics-color=always)
else()
    message(STATUS "No colored compiler diagnostic set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
endif()


  function(my_set_project_compile_options project_name)

    set(COMMON_OPTIONS
    )

    set(DEBUG_OPTIONS
            -g
            -fsanitize=address,undefined,leak
    )

    set(RELEASE_OPTIONS
            -O3
    )

    target_compile_options(
            ${project_name}
            INTERFACE
            ${COMMON_OPTIONS}
            $<$<CONFIG:Debug>:${DEBUG_OPTIONS}>
            $<$<CONFIG:Release>:${RELEASE_OPTIONS}>
    )

    target_precompile_headers(
            ${project_name}
            INTERFACE
            <vector>
            <string>
            <utility>
            <memory>
    )
endfunction()
