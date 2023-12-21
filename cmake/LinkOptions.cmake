function(my_set_project_link_options project_name)

    set(COMMON_OPTIONS
    )

    set(DEBUG_OPTIONS
            -fsanitize=address,undefined,leak
    )

    set(RELEASE_OPTIONS
    )

    target_link_options(
            ${project_name}
            INTERFACE
            ${COMMON_OPTIONS}
            $<$<CONFIG:Debug>:${DEBUG_OPTIONS}>
            $<$<CONFIG:Release>:${RELEASE_OPTIONS}>
    )
endfunction()
