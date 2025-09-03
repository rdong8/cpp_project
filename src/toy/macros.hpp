#include "llvm/Support/raw_ostream.h"

// Macros you want to share between modules have to be defined in a header
#define TOY_ASSERT(expr, msg)                                           \
    do                                                                  \
    {                                                                   \
        if (!(expr)) [[unlikely]]                                       \
        {                                                               \
            constexpr auto location = std::source_location::current();  \
            llvm::errs().changeColor(llvm::raw_ostream::RED)            \
                << location.file_name() << ':'                          \
                << location.function_name() << ':'                      \
                << location.line()                                      \
                << ':' << location.column();                            \
            llvm::errs().resetColor()                                   \
                << " Assertion failed: " #expr ": " << msg << '\n';     \
            std::abort();                                               \
        }                                                               \
    } while (false)

// Ensures that std::source_location is available to TOY_ASSERT users
import std;
