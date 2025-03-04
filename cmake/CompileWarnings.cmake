# Credit: https://github.com/cpp-best-practices/cmake_template/blogit b/main/cmake/CompilerWarnings.cmake
# https://best.openssf.org/Compiler-Hardening-Guides/Compiler-Options-Hardening-Guide-for-C-and-C++.html

function(set_project_warnings project_name)
    set(CLANG_WARNINGS
            "-fcolor-diagnostics"
    )

    set(GCC_WARNINGS
            "-fdiagonostics-color=always"
            "-Wduplicated-branches" # Warn if if / else branches have duplicated code
            "-Wduplicated-cond" # Warn if if / else chain has duplicated conditions
            "-Wlogical-op" # Warn about logical operations being used where bitwise were probably wanted
            "-Wmisleading-indentation" # Warn if indentation implies blocks where blocks do not exist
            "-Wuseless-cast" # Warn if you perform a cast to the same type
    )

    target_compile_options("${project_name}"
            INTERFACE
            "$<$<CXX_COMPILER_ID:Clang>:${CLANG_WARNINGS}>"
            "$<$<CXX_COMPILER_ID:GCC>:${GCC_WARNINGS}>"
            "-Wall"
            "-Wcast-align" # Warn for potential performance problem casts
            "-Wconversion" # Warn on type conversions that may lose data
            "-Wdouble-promotion" # Warn if float is implicit promoted to double
            "-Werror"
            "-Wextra" # Reasonable and standard
            "-Wformat=2" # Warn on security issues around functions that format output (ie printf)
            "-Wimplicit-fallthrough" # Warn on statements that fallthrough without an explicit annotation
            "-Wnon-virtual-dtor" # Warn the user if a class with virtual functions has a non-virtual destructor. This helps catch hard to track down memory errors
            "-Wnull-dereference" # Warn if a null dereference is detected
            "-Wold-style-cast" # Warn for c-style casts
            "-Woverloaded-virtual" # Warn if you overload (not override) a virtual function
            "-Wpedantic" # Warn if non-standard C++ is used
            "-Wshadow" # Warn the user if a variable declaration shadows one from a parent context
            "-Wsign-conversion" # Warn on sign conversions
            "-Wunused" # Warn on anything being unused
    )
endfunction()
