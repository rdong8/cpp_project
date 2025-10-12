module;

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorOr.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"

export module main;

import toyc;

import std;

namespace cl = llvm::cl;

namespace
{

enum class Action
{
    None,
    DumpAST,
};

cl::opt<std::string> input_filename{cl::Positional, cl::desc("<input toy file>"), cl::init("-"),
                                    cl::value_desc("filename")};

cl::opt<Action> emit_action{"emit", cl::desc("Select the kind of output desired"),
                            cl::values(clEnumValN(Action::DumpAST, "ast", "output the AST dump"))};

auto parse_input_file(llvm::StringRef filename) -> std::unique_ptr<toy::ModuleAST>
{
    llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> file_or_error = llvm::MemoryBuffer::getFileOrSTDIN(filename);

    if (std::error_code const ec = file_or_error.getError())
    {
        llvm::errs() << "Could not open input file: " << ec.message() << '\n';
        return nullptr;
    }

    auto buffer = file_or_error.get()->getBuffer();
    toy::LexerBuffer lexer{buffer.begin(), buffer.end(), std::string{filename}};
    toy::Parser parser{lexer};
    return parser.parseModule();
}

} // namespace

auto main(int argc, char *argv[]) -> int
{
    cl::ParseCommandLineOptions{argc, argv, "toy compiler\n"};

    auto module_ast = parse_input_file(input_filename);

    if (!module_ast)
    {
        return 1;
    }

    switch (emit_action)
    {
    case Action::DumpAST:
    {
        toy::dump(*module_ast);
        return 0;
    }
    case Action::None:
    {
        llvm::errs() << "No action specified (parsing only?), use -emit=<action>\n";
        return 1;
    }
    }

    std::unreachable();
}
