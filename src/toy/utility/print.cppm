module;

#include <llvm/Support/FormatVariadic.h>
#include <llvm/Support/raw_ostream.h>

export module toyc.utility.print;

import std;

export namespace toyc
{

template <typename... Args> auto print(llvm::raw_ostream &stream, std::string_view fmt, Args &&...args) -> void
{
    llvm::formatv(fmt.data(), std::forward<Args>(args)...).format(stream);
}

template <typename... Args> auto print(std::string_view fmt, Args &&...args) -> void
{
    print(llvm::outs(), fmt, std::forward<Args>(args)...);
}

template <typename... Args> auto eprint(std::string_view fmt, Args &&...args) -> void
{
    print(llvm::errs(), fmt, std::forward<Args>(args)...);
}

template <typename... Args> auto println(llvm::raw_ostream &stream, std::string_view fmt, Args &&...args) -> void
{
    print(stream, fmt, std::forward<Args>(args)...);
    stream.write('\n');
}

template <typename... Args> auto println(std::string_view fmt, Args &&...args) -> void
{
    println(llvm::outs(), fmt, std::forward<Args>(args)...);
}

template <typename... Args> auto eprintln(std::string_view fmt, Args &&...args) -> void
{
    println(llvm::errs(), fmt, std::forward<Args>(args)...);
}

} // namespace toyc
