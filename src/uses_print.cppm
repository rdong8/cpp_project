module;

// This header must be included to surface the error
#include <llvm/Support/FormatProviders.h>

export module uses_print;

import print;

auto foo(llvm::raw_ostream &) -> void
{
}
