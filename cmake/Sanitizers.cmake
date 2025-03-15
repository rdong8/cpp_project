option(ENABLE_ASAN "Enable address sanitizer" OFF)
option(ENABLE_LSAN "Enable leak sanitizer" OFF)
option(ENABLE_MSAN "Enable memory sanitizer" OFF)
option(ENABLE_RTSAN "Enable realtime sanitizer" OFF)
option(ENABLE_SAFE_STACK "Enable safe stack" OFF)
option(ENABLE_TSAN "Enable thread sanitizer" OFF)
option(ENABLE_UBSAN "Enable undefined behaviour sanitizer" OFF)

set(DEFAULT_DEBUG_SANITIZERS "address,leak,undefined")
set(DEFAULT_RELWITHDEBINFO_SANITIZERS "safe-stack")
set(DEFAULT_RELEASE_SANITIZERS "safe-stack")

macro(set_project_sanitizers project_name)
    if ("${ENABLE_ASAN}")
        list(APPEND SANITIZERS "address")
    endif ()

    if ("${ENABLE_LSAN}")
        list(APPEND SANITIZERS "leak")
    endif ()

    if ("${ENABLE_MSAN}")
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            message(SEND_ERROR "Memory sanitizer doesn't work with GCC")
        endif ()
        list(APPEND SANITIZERS "memory")
    endif ()

    if ("${ENABLE_RTSAN}")
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            message(SEND_ERROR "Realtime sanitizer doesn't work with GCC")
        endif ()
        list(APPEND SANITIZERS "realtime")
    endif ()

    if ("${ENABLE_SAFE_STACK}")
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            message(SEND_ERROR "Safe stack doesn't work with GCC")
        endif ()
        list(APPEND SANITIZERS "safe-stack")
    endif ()

    if ("${ENABLE_TSAN}")
        list(APPEND SANITIZERS "thread")
    endif ()

    if ("${ENABLE_UBSAN}")
        list(APPEND SANITIZERS "undefined")
    endif ()

    list(JOIN SANITIZERS "," LIST_OF_SANITIZERS)

    if (LIST_OF_SANITIZERS AND (NOT "${LIST_OF_SANITIZERS}" STREQUAL ""))
        target_compile_options("${project_name}" INTERFACE "-fsanitize=${LIST_OF_SANITIZERS}")
        target_link_options("${project_name}" INTERFACE "-fsanitize=${LIST_OF_SANITIZERS}")
    endif ()

    if (DEFAULT_DEBUG_SANITIZERS AND (NOT "${DEFAULT_DEBUG_SANITIZERS}" STREQUAL ""))
        target_compile_options("${project_name}"
            INTERFACE
            "$<$<CONFIG:Debug>:-fsanitize=${DEFAULT_DEBUG_SANITIZERS}>"
        )
        target_link_options("${project_name}"
            INTERFACE
            "$<$<CONFIG:Debug>:-fsanitize=${DEFAULT_DEBUG_SANITIZERS}>"
        )
    endif ()

    if (DEFAULT_RELWITHDEBINFO_SANITIZERS AND (NOT "${DEFAULT_RELWITHDEBINFO_SANITIZERS}" STREQUAL ""))
        target_compile_options("${project_name}"
            INTERFACE
            "$<$<CONFIG:RelWithDebInfo>:-fsanitize=${DEFAULT_RELWITHDEBINFO_SANITIZERS}>"
        )
        target_link_options("${project_name}"
            INTERFACE
            "$<$<CONFIG:RelWithDebInfo>:-fsanitize=${DEFAULT_RELWITHDEBINFO_SANITIZERS}>"
        )
    endif ()

    if (DEFAULT_RELEASE_SANITIZERS AND (NOT "${DEFAULT_RELEASE_SANITIZERS}" STREQUAL ""))
        target_compile_options("${project_name}"
            INTERFACE
            "$<$<CONFIG:Release>:-fsanitize=${DEFAULT_RELEASE_SANITIZERS}>"
        )
        target_link_options("${project_name}"
            INTERFACE
            "$<$<CONFIG:Release>:-fsanitize=${DEFAULT_RELEASE_SANIZERS}>"
        )
    endif ()
endmacro()
