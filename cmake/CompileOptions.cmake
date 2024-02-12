# Global options Compiler specific options
add_compile_options("$<$<CXX_COMPILER_ID:Clang>:-stdlib=libc++>")

function(set_project_compile_options project_name)
  set(COMMON_OPTIONS)

  set(DEBUG_OPTIONS
      "-fsanitize=address,undefined,leak" "$<$<CXX_COMPILER_ID:Clang>:-glldb>"
      "$<$<CXX_COMPILER_ID:GCC>:-ggdb>")

  set(RELEASE_OPTIONS "-march=native" "-O3")

  target_compile_options(
    ${project_name}
    INTERFACE ${COMMON_OPTIONS} $<$<CONFIG:Debug>:${DEBUG_OPTIONS}>
              $<$<CONFIG:Release>:${RELEASE_OPTIONS}>)
endfunction()
