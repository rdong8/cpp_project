// TODO: make it a header unit, no CMake support
// https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html#limitations

// Dependent modules:
// - std

#include "llvm/Support/raw_ostream.h"

// Macros you want to share between modules have to be defined in a header
#define TOY_ASSERT(expr, msg)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(expr)) [[unlikely]]                                                                                      \
        {                                                                                                              \
            constexpr auto location = std::source_location::current();                                                 \
            llvm::errs().changeColor(llvm::raw_ostream::RED)                                                           \
                << location.file_name() << ':' << location.function_name() << ':' << location.line() << ':'            \
                << location.column();                                                                                  \
            llvm::errs().resetColor() << " Assertion `" << #expr << "` failed: " << msg << '\n';                       \
            std::abort();                                                                                              \
        }                                                                                                              \
    } while (false)
