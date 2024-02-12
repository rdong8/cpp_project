# Global options Compiler specific options
add_link_options(
  "$<$<CXX_COMPILER_ID:Clang>:-fuse-ld=lld;-lc++abi;-stdlib=libc++>")

function(set_project_link_options project_name)
  set(COMMON_OPTIONS)

  set(DEBUG_OPTIONS "-fsanitize=address,leak,undefined")

  set(RELEASE_OPTIONS)

  target_link_options(
    ${project_name} INTERFACE ${COMMON_OPTIONS}
    $<$<CONFIG:Debug>:${DEBUG_OPTIONS}> $<$<CONFIG:Release>:${RELEASE_OPTIONS}>)
endfunction()
