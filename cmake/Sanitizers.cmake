option(ENABLE_ASAN "Enable address sanitizer" OFF)
option(ENABLE_LSAN "Enable leak sanitizer" OFF)
option(ENABLE_MSAN "Enable memory sanitizer" OFF)
option(ENABLE_TSAN "Enable thread sanitizer" OFF)
option(ENABLE_UBSAN "Enable undefined behaviour sanitizer" OFF)

macro(set_project_sanitizers project_name)

    if ("${ENABLE_ASAN}")
        list(APPEND SANITIZERS "address")
    endif ()

    if ("${ENABLE_LSAN}")
        list(APPEND SANITIZERS "leak")
    endif ()

    if ("${ENABLE_TSAN}")
        if ("address" IN_LIST SANITIZERS OR "leak" IN_LIST SANITIZERS)
            message(SEND_ERROR "Thread sanitizer does not work with address or leak sanitizer")
        endif ()
        list(APPEND SANITIZERS "thread")
    endif ()

    if ("${ENABLE_MSAN}")
        if ("address" IN_LIST SANITIZERS
                OR "leak" IN_LIST SANITIZERS
                OR "thread" IN_LIST SANITIZERS)
            message(SEND_ERROR "Memory sanitizer doesn't work with address, leak or thread sanitizer")
        elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            message(SEND_ERROR "Memory sanitizer doesn't work with GCC")
        endif ()
        list(APPEND SANITIZERS "memory")
    endif ()

    if ("${ENABLE_UBSAN}")
        list(APPEND SANITIZERS "undefined")
    endif ()

    list(JOIN SANITIZERS "," LIST_OF_SANITIZERS)

    if (LIST_OF_SANITIZERS AND (NOT "${LIST_OF_SANITIZERS}" STREQUAL ""))
        target_compile_options("${project_name}" INTERFACE "-fsanitize=${LIST_OF_SANITIZERS}")
        target_link_options("${project_name}" INTERFACE "-fsanitize=${LIST_OF_SANITIZERS}")
    endif ()
endmacro()
