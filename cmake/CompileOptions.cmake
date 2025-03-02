function(set_project_compile_options project_name)
    set(DEBUG_INFO_OPTION
            "$<$<CXX_COMPILER_ID:Clang>:-glldb>"
            "$<$<CXX_COMPILER_ID:GCC>:-ggdb>"
    )

    set(DEBUG_OPTIONS
            "${DEBUG_INFO_OPTION}"
            "${DEFAULT_SANITIZERS}"
            "-D_FORTIFY_SOURCE=3"
            "-Og"
            "-U_FORTIFY_SOURCE"
    )

    set(RELWITHDEBINFO_OPTIONS
            "${DEBUG_INFO_OPTION}"
            "-D_FORTIFY_SOURCE=1"
            "-O2"
    )

    set(RELEASE_OPTIONS
            "-D_FORTIFY_SOURCE=1"
            "-O3"
            "-march=native"
    )

    target_compile_options("${project_name}"
            INTERFACE
            "$<$<CXX_COMPILER_ID:Clang>:-fsanitize=safe-stack>"
            "$<$<CONFIG:Debug>:${DEBUG_OPTIONS}>"
            "$<$<CONFIG:Release>:${RELEASE_OPTIONS}>"
            "$<$<CONFIG:RelWithDebInfo>:${RELWITHDEBINFO_OPTIONS}>"
    )
endfunction()
