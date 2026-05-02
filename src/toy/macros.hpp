/// @file macros.hpp
/// Macros you want to share between modules have to be defined in a header

// TODO: make it a header unit, no CMake support
// https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html#limitations

// Dependent modules:
// - std
// - toyc.utility

#include <cpptrace/cpptrace.hpp>
#include <llvm/Support/raw_ostream.h>

#define TOYC_ASSERT_IMPL(expr, fmt, ...)                                                                               \
    [&]                                                                                                                \
    {                                                                                                                  \
        if (!(expr)) [[unlikely]]                                                                                      \
        {                                                                                                              \
            toyc::eprintln(fmt __VA_OPT__(, ) __VA_ARGS__);                                                            \
            toyc::eprintln("{}", toyc::trace_formatter.format(cpptrace::generate_trace()));                            \
            std::terminate();                                                                                          \
        }                                                                                                              \
    }()

#define TOYC_ASSERT(expr, fmt, ...)                                                                                    \
    TOYC_ASSERT_IMPL(expr, "Assertion `{}` failed: " fmt, #expr __VA_OPT__(, ) __VA_ARGS__)

#define TOYC_ASSERT_FALSE(fmt, ...)                                                                                    \
    TOYC_ASSERT(false, fmt __VA_OPT__(, ) __VA_ARGS__);                                                                \
    std::unreachable()

#define TOYC_TODO(fmt, ...)                                                                                            \
    TOYC_ASSERT_IMPL(false, "TODO:" fmt __VA_OPT__(, ) __VA_ARGS__);                                                   \
    std::unreachable()
