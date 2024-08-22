macro(set_project_link_options project_name)
#    https://cmake.org/cmake/help/latest/variable/CMAKE_LINKER_TYPE.html
#    set_property(TARGET "${project_name}" PROPERTY LINKER_TYPE "MOLD")
    # Workaround for now - not sure how to set per-target linker
    set(CMAKE_LINKER_TYPE "MOLD")
endmacro()
