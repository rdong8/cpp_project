# Global options Compiler specific options
if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  add_compile_options("-stdlib=libc++")
endif()

function(set_project_compile_options project_name)

  set(COMMON_OPTIONS)

  set(DEBUG_OPTIONS "-g" "-fsanitize=address,undefined,leak")

  set(RELEASE_OPTIONS -O3)

  target_compile_options(
    ${project_name}
    INTERFACE ${COMMON_OPTIONS} $<$<CONFIG:Debug>:${DEBUG_OPTIONS}>
              $<$<CONFIG:Release>:${RELEASE_OPTIONS}>)
endfunction()
