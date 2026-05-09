module;

#include <utility>

#include <llvm/ADT/StringRef.h>
#include <llvm/Support/FormatVariadic.h>
#include <llvm/Support/raw_ostream.h>

export module print;

template <typename... Args> auto println(llvm::raw_ostream &stream, llvm::StringRef fmt, Args &&...args) -> void
{
    llvm::formatv(fmt.data(), std::forward<Args>(args)...).format(stream);
    stream << '\n';
}
