include(LLVM.cmake)

add_library(toyc_macros)
target_sources(toyc_macros
    PUBLIC FILE_SET HEADERS FILES
        macros.hpp
)
target_link_libraries(toyc_macros
    PRIVATE
        config
        llvm_config
)

add_library(toyc_lexer)
target_sources(toyc_lexer
    PUBLIC FILE_SET CXX_MODULES FILES
        lexer.cppm
)
target_link_libraries(toyc_lexer
    PRIVATE
        config
        llvm_config
        toyc_macros
)

add_library(toyc_ast)
target_sources(toyc_ast
    PUBLIC FILE_SET CXX_MODULES FILES
        ast/mod.cppm
    PRIVATE
        ast/dumper.cppm
)
target_link_libraries(toyc_ast
    PRIVATE`
        config
        llvm_config
        toyc_lexer
)

add_library(toyc_parser)
target_sources(toyc_parser
    PUBLIC FILE_SET CXX_MODULES FILES
        parser.cppm
)
target_link_libraries(toyc_parser
    PRIVATE
        config
        llvm_config
)

add_library(toyc_lib)
target_sources(toyc_lib
    PUBLIC FILE_SET CXX_MODULES FILES
        lib.cppm
)
target_link_libraries(toyc_lib
    PRIVATE
        config
        llvm_config
        toyc_ast
        toyc_lexer
        toyc_macros
        toyc_parser
)

add_executable(toyc)
target_sources(toyc
    PUBLIC FILE_SET CXX_MODULES FILES
        main.cppm
)
target_link_libraries(toyc
    PRIVATE
        config
        llvm_config
        toyc_lib
)
