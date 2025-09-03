
include(LLVM.cmake)

# Run `llvm-config --components` to see all available components
llvm_map_components_to_libnames(llvm_libs support)

add_library(toyc_macros)
target_sources(toyc_macros
        PUBLIC FILE_SET HEADERS FILES
        macros.hpp
)
target_link_libraries(toyc_macros
        PRIVATE 
        config
        llvm_config
        ${llvm_libs}
)

add_executable(toyc_lexer)
target_sources(toyc_lexer
    PUBLIC FILE_SET CXX_MODULES FILES
    lexer.cppm
)
target_link_libraries(toyc_lexer
        PRIVATE 
        config
        llvm_config
        ${llvm_libs}
        MLIRSupport
)

add_executable(toyc)
target_sources(toyc
    PUBLIC FILE_SET CXX_MODULES FILES
    toyc.cppm
)
target_link_libraries(toyc
        PRIVATE 
        config
        llvm_config
        ${llvm_libs}
        MLIRSupport
)

# add_executable(tmp)
# target_sources(tmp
#         PUBLIC FILE_SET CXX_MODULES FILES
#         tmp.cppm
# )
# target_link_libraries(tmp
#         PRIVATE 
#         config
#         llvm_config
#         toyc_macros
#         ${llvm_libs}
#         MLIRSupport
# )

add_executable(tmp_non_module)
target_sources(tmp_non_module
        PRIVATE
        tmp.cpp
)
target_link_libraries(tmp_non_module
        PRIVATE 
        config
        llvm_config
        toyc_macros
        ${llvm_libs}
        MLIRSupport
)
