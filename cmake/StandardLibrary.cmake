option(ENABLE_LIBCXX "Use libc++ if compiler if Clang" ON)

macro(set_project_stdlib project_name)
    if (${ENABLE_LIBCXX})
        target_compile_options("${project_name}" INTERFACE "$<$<CXX_COMPILER_ID:Clang>:-stdlib=libc++>")
        target_link_options("${project_name}" INTERFACE "$<$<CXX_COMPILER_ID:Clang>:-lc++abi;-stdlib=libc++>")
    endif ()
endmacro()
