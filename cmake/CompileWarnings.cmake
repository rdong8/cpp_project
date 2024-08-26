# Credit: https://github.com/cpp-best-practices/cmake_template/blob/main/cmake/CompilerWarnings.cmake
# https://best.openssf.org/Compiler-Hardening-Guides/Compiler-Options-Hardening-Guide-for-C-and-C++.html

function(set_project_warnings project_name)
    set(CLANG_WARNINGS
            "-fcolor-diagnostics"
    )

    set(GCC_WARNINGS
            "-fdiagonostics-color=always"
            "-Wduplicated-branches" # warn if if / else branches have duplicated code
            "-Wduplicated-cond" # warn if if / else chain has duplicated conditions
            "-Wlogical-op" # warn about logical operations being used where bitwise were probably wanted
            "-Wmisleading-indentation" # warn if indentation implies blocks where blocks do not exist
            "-Wuseless-cast" # warn if you perform a cast to the same type
    )

    target_compile_options("${project_name}"
            INTERFACE
            "$<$<CXX_COMPILER_ID:Clang>:${CLANG_WARNINGS}>"
            "$<$<CXX_COMPILER_ID:GCC>:${GCC_WARNINGS}>"
            "-Wall"
            "-Wcast-align" # warn for potential performance problem casts
            "-Wconversion" # warn on type conversions that may lose data
            "-Wdouble-promotion" # warn if float is implicit promoted to double
            "-Werror"
            "-Wextra" # reasonable and standard
            "-Wformat=2" # warn on security issues around functions that format output (ie printf)
            "-Wimplicit-fallthrough" # warn on statements that fallthrough without an explicit annotation
            "-Wnon-virtual-dtor" # warn the user if a class with virtual functions has a non-virtual destructor. This
            # helps catch hard to track down memory errors
            "-Wnull-dereference" # warn if a null dereference is detected
            "-Wold-style-cast" # warn for c-style casts
            "-Woverloaded-virtual" # warn if you overload (not override) a virtual function
            "-Wpedantic" # warn if non-standard C++ is used
            "-Wshadow" # warn the user if a variable declaration shadows one from a parent context
            "-Wsign-conversion" # warn on sign conversions
            "-Wunused" # warn on anything being unused
    )
endfunction()
