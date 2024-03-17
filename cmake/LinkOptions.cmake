function(set_project_link_options project_name)
    target_link_options("${project_name}"
            INTERFACE
            "$<$<CONFIG:Debug>:${DEFAULT_SANITIZERS}>"
    )
endfunction()
