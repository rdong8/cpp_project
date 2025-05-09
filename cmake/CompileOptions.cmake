function(set_project_compile_options project_name)
    set(DEBUG_INFO_OPTION
            "$<$<CXX_COMPILER_ID:Clang>:-glldb>"
            "$<$<CXX_COMPILER_ID:GCC>:-ggdb>"
    )

    set(DEBUG_OPTIONS
            "${DEBUG_INFO_OPTION}"
            "-D_FORTIFY_SOURCE=3"
            "-O0"
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
            "$<$<CONFIG:Debug>:${DEBUG_OPTIONS}>"
            "$<$<CONFIG:Release>:${RELEASE_OPTIONS}>"
            "$<$<CONFIG:RelWithDebInfo>:${RELWITHDEBINFO_OPTIONS}>"
    )
endfunction()
