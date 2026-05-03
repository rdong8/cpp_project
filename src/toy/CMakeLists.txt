find_package(cpptrace REQUIRED CONFIG)
find_package(magic_enum REQUIRED CONFIG)

include(cmake/LLVM.cmake)

add_library(toyc.config)
target_sources(toyc.config
    PUBLIC FILE_SET HEADERS FILES
        macros.hpp
)
target_link_libraries(toyc.config
    INTERFACE
        cpptrace::cpptrace
        config
        llvm_config
)

add_module_library(toyc.utility utility)
target_link_libraries(toyc.utility
    PRIVATE
        cpptrace::cpptrace
        toyc.config
        llvm_config
)

add_module_library(toyc.lexer lexer)
target_link_libraries(toyc.lexer
    PRIVATE
        magic_enum::magic_enum
        toyc.config
        toyc.utility
)

add_module_library(toyc.parser parser)
target_link_libraries(toyc.parser
    PRIVATE
        magic_enum::magic_enum
        toyc.config
        toyc.lexer
        toyc.utility
)

add_module_executable(toyc .)
target_link_libraries(toyc
    PRIVATE
        toyc.config
        toyc.lexer
        toyc.parser
        toyc.utility
)
