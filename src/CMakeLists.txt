include(cmake/LLVM.cmake)

add_library(print)
target_sources(print
    PUBLIC FILE_SET CXX_MODULES FILES
        print.cppm
)
target_link_libraries(print
    PRIVATE
        config
        llvm_config
)

add_library(uses_print)
target_sources(uses_print
    PUBLIC FILE_SET CXX_MODULES FILES
        uses_print.cppm
)
target_link_libraries(uses_print
    PRIVATE
        config
        llvm_config
        print
)
