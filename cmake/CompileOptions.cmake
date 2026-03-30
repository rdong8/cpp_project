function(set_project_compile_options project_name)
    set(DEBUGGING_OPTIONS
            "$<$<CXX_COMPILER_ID:Clang>:-glldb>"
            "$<$<CXX_COMPILER_ID:GCC>:-ggdb>"
            "-fno-omit-frame-pointer"
            "-fno-sanitize-merge"
        # Turn on this flag only if you need to run LLDB
        # It interferes with stack traces
        # TODO: remove disclaimer when this is resolved https://github.com/jeremy-rifkin/cpptrace/issues/307
            "-gmodules"
    )

    set(DEBUG_OPTIONS
            "${DEBUGGING_OPTIONS}"
            "-D_FORTIFY_SOURCE=3"
            "-Og"
            "-U_FORTIFY_SOURCE"
    )

    set(RELWITHDEBINFO_OPTIONS
            "${DEBUGGING_OPTIONS}"
            "-D_FORTIFY_SOURCE=1"
            "-O2"
    )

    set(RELEASE_OPTIONS
            "-D_FORTIFY_SOURCE=1"
            "-O3"
            "-march=native"
        #     Enable if profiling
        #     "-pg"
    )

    target_compile_options("${project_name}"
            INTERFACE
            "$<$<CONFIG:Debug>:${DEBUG_OPTIONS}>"
            "$<$<CONFIG:Release>:${RELEASE_OPTIONS}>"
            "$<$<CONFIG:RelWithDebInfo>:${RELWITHDEBINFO_OPTIONS}>"
    )
endfunction()
