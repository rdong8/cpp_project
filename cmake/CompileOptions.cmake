function(set_project_compile_options project_name)
    set(DEBUG_INFO_OPTION
            "$<$<CXX_COMPILER_ID:Clang>:-glldb>"
            "$<$<CXX_COMPILER_ID:GCC>:-ggdb>"
    )

    set(DEBUG_OPTIONS
            "${DEBUG_INFO_OPTION}"
            "${DEFAULT_SANITIZERS}"
            "-Og"
    )

    set(RELWITHDEBINFO_OPTIONS
            "${DEBUG_INFO_OPTION}"
            "-O2"
    )

    set(RELEASE_OPTIONS
            "-march=native"
            "-O3"
    )

    target_compile_options("${project_name}"
            INTERFACE
            "$<$<CONFIG:Debug>:${DEBUG_OPTIONS}>"
            "$<$<CONFIG:Release>:${RELEASE_OPTIONS}>"
            "$<$<CONFIG:RelWithDebInfo>:${RELWITHDEBINFO_OPTIONS}>"
    )
endfunction()
