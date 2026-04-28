macro(add_module_helper name subpath)
    set(LIBRARY_SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${subpath}")
    file(GLOB MODULE_FILES "${LIBRARY_SOURCE_PATH}/*.cppm")
    file(GLOB IMPLEMENTATION_FILES "${LIBRARY_SOURCE_PATH}/*.cpp")
    file(GLOB HEADERS "${LIBRARY_SOURCE_PATH}/*.hpp")

    target_link_libraries("${name}"
        PRIVATE
            config
    )
endmacro()

# This function adds a new module library to the project whose lib.cppm is
# located at the specified path relative to ${CMAKE_CURRENT_SOURCE_DIR}
# It should not be used for executables, non-module libraries, etc.
# If you add a new file, you have to rerun the cmake config (`just config`)
# It will *not* automatically be picked up here
# Add non-module files (".hpp" headers) manually via `target_sources`
function(add_module_library name subpath)
    add_library("${name}")
    add_module_helper("${name}" "${subpath}")

    set(MAIN_FILE "${IMPLEMENTATION_FILES}")
    list(FILTER MAIN_FILE INCLUDE REGEX ".*main\.cpp")

    if(MAIN_FILE)
        message(FATAL_ERROR "Error: module library ${name} cannot contain main")
    endif()

    target_sources("${name}"
        PUBLIC
            FILE_SET CXX_MODULES FILES
                ${MODULE_FILES}
            FILE_SET HEADERS FILES
                ${HEADERS}
        PRIVATE
            ${IMPLEMENTATION_FILES}
    )
endfunction()

function(add_module_executable name subpath)
    add_executable("${name}")
    add_module_helper("${name}" "${subpath}")

    set(MAIN_FILE "${IMPLEMENTATION_FILES}")
    list(FILTER MAIN_FILE INCLUDE REGEX ".*main\.cpp")
    list(FILTER IMPLEMENTATION_FILES EXCLUDE REGEX ".*main\.cpp")

    target_sources("${name}"
        PRIVATE
            FILE_SET CXX_MODULES FILES
                ${MODULE_FILES}
            FILE_SET HEADERS FILES
                ${HEADERS}
            ${IMPLEMENTATION_FILES}
        PUBLIC
            ${MAIN_FILE}
    )
endfunction()
