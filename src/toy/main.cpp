#include <llvm/ADT/StringRef.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/ErrorOr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/raw_ostream.h>

import std;

import toyc.lexer;
import toyc.parser;
import toyc.utility;

namespace cl = llvm::cl;

namespace
{

enum class Action
{
    None,
    DumpAST,
};

cl::opt<std::string> input_filename{
    cl::Positional,               //
    cl::desc{"<input toy file>"}, //
    cl::init("-"),                //
    cl::value_desc{"filename"}    //
};

cl::opt<Action> emit_action{
    "emit",                                                               //
    cl::desc{"Select the kind of output desired"},                        //
    cl::values(clEnumValN(Action::DumpAST, "ast", "output the AST dump")) //
};

[[nodiscard]]
auto parse_input_file(llvm::StringRef filename) -> toyc::ASTPtr<toyc::ModuleAST>
{
    auto const file_or_error{llvm::MemoryBuffer::getFileOrSTDIN(filename)};

    if (auto const ec{file_or_error.getError()})
    {
        toyc::eprintln("Could not open input file: {}", ec.message());
        return nullptr;
    }

    auto buffer{file_or_error.get()->getBuffer()};
    toyc::LexerBuffer lexer{buffer.begin(), buffer.end(), std::string{filename}};
    toyc::Parser parser{lexer};
    return parser.parse_module();
}

} // namespace

auto main(int argc, char *argv[]) -> int
{
    cl::ParseCommandLineOptions(argc, argv, "toy compiler\n");

    auto module_ast{parse_input_file(input_filename)};

    if (!module_ast)
    {
        return 1;
    }

    switch (emit_action)
    {
        case Action::DumpAST:
        {
            toyc::dump(*module_ast);
            return 0;
        }
        case Action::None:
        {
            toyc::eprintln("No action specified (parsing only?), use -emit=<action>");
            return 1;
        }
    }
}
