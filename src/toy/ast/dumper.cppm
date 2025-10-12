module;

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/Twine.h"
#include "llvm/ADT/TypeSwitch.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

module toyc.ast;

import toyc.lexer;

import std;

namespace toy
{

namespace
{

/// RAII helper to manage increasing/decreasing indentation as we traverse the AST
struct Indent
{
    Indent(int &level) : level{level} { ++this->level; }
    ~Indent() { --this->level; }

    int &level;
};

auto loc(auto const *node) -> auto
{
    Location const &loc{node->loc()};
    return llvm::Twine{'@'} + *loc.filename + ':' + llvm::Twine{loc.line} + ':' + llvm::Twine{loc.col};
}

// Helper macro to bump the indentation level and print the leading spaces for the current indentations
#define INDENT()                                                                                                       \
    Indent level_(self.current_indent);                                                                                \
    self.indent();

/// Helper class that implements the AST tree traversal and prints the nodes along the way. The only data member is the
/// current indentation level.
class ASTDumper
{
  public:
    /// Print a module, prints the functions in sequence
    auto dump(this ASTDumper &self, ModuleAST const *node) -> void
    {
        INDENT();
        llvm::errs() << "Module:\n";

        for (auto const &func : *node)
        {
            self.dump(&func);
        }
    }

  private:
    /// Actually print spaces matching the current indentation level
    auto indent(this ASTDumper const &self) -> void
    {
        for (auto const _ : std::views::iota(0, self.current_indent))
        {
            llvm::errs() << ' ';
        }
    }

    /// Print type: only the shape is printed between the '<' and '>'
    auto dump(this ASTDumper &self, VarType const &type) -> void
    {
        llvm::errs() << '<';
        llvm::interleaveComma(type.shape, llvm::errs());
        llvm::errs() << '>';
    }

    /// Dispatch to generic expressions to the appropriate subclass using RTTI
    auto dump(this ASTDumper &self, ExprAST const *expr) -> void
    {
        llvm::TypeSwitch<ExprAST const *>(expr)
            .Case<BinaryExprAST, CallExprAST, LiteralExprAST, NumberExprAST, PrintExprAST, ReturnExprAST,
                  VarDeclExprAST, VariableExprAST>([&](auto *node) { self.dump(node); })
            .Default(
                [&](ExprAST *)
                {
                    INDENT();
                    llvm::errs() << "<unknown Expr, kind " << std::to_underlying(expr->get_kind()) << ">\n";
                });
    }

    /// A "block", or list of expressions
    auto dump(this ASTDumper &self, ExprASTList const *expr_list) -> void
    {
        INDENT();
        llvm::errs() << "Block {\n";

        for (auto const &expr : *expr_list)
        {
            self.dump(expr.get());
        }

        self.indent();
        llvm::errs() << "} // Block\n";
    }

    /// A literal number, just print the value
    auto dump(this ASTDumper &self, NumberExprAST const *num) -> void
    {
        INDENT();
        llvm::errs() << num->get_value() << ' ' << loc(num) << '\n';
    }

    /// Print a literal, see above for implementation
    auto dump(this ASTDumper &self, LiteralExprAST const *node) -> void
    {
        INDENT();
        self.print_literal_helper(node);
        llvm::errs() << ' ' << loc(node) << '\n';
    }

    /// Print a variable reference (just a name)
    auto dump(this ASTDumper &self, VariableExprAST const *node) -> void
    {
        INDENT();
        llvm::errs() << "var: " << node->get_name() << ' ' << loc(node) << '\n';
    }

    /// A variable declaration is printing the variable name, type and then recurse on the initializer
    auto dump(this ASTDumper &self, VarDeclExprAST const *var_decl) -> void
    {
        INDENT();
        llvm::errs() << "VarDecl " << var_decl->get_name();
        self.dump(var_decl->get_type());
        llvm::errs() << ' ' << loc(var_decl) << '\n';
        self.dump(var_decl->get_initializer());
    }

    /// Print the return and its optional argument
    auto dump(this ASTDumper &self, ReturnExprAST const *node) -> void
    {
        INDENT();
        llvm::errs() << "Return\n";

        if (auto maybe_expr{node->get_expr().has_value()}; maybe_expr.has_value())
        {
            return self.dump(*maybe_expr);
        }

        {
            INDENT();
            llvm::errs() << "(void)\n";
        }
    }

    /// Print a binary operation, first the operator and then recurse into LHS and RHS
    auto dump(this ASTDumper &self, BinaryExprAST const *node) -> void
    {
        INDENT();
        llvm::errs() << "BinOp: " << node->get_op() << ' ' << loc(node) << '\n';
        self.dump(node->get_lhs());
        self.dump(node->get_rhs());
    }

    /// Print a call expression, first the callee name and then the list of args
    auto dump(this ASTDumper &self, CallExprAST const *node) -> void
    {
        INDENT();
        llvm::errs() << "Call '" << node->get_callee() << "' [" << loc(node) << '\n';

        for (auto const &arg : node->get_args())
        {
            self.dump(arg.get());
        }

        self.indent();
        llvm::errs() << "]\n";
    }

    /// Print a builtin print call, first the builtin name and then the argument
    auto dump(this ASTDumper &self, PrintExprAST const *node) -> void
    {
        INDENT();
        llvm::errs() << "Print [" << loc(node) << '\n';
        self.dump(node->get_arg());
        self.indent();
        llvm::errs() << "]\n";
    }

    /// Helper to recursively print a literal. Handles nested arrays like:
    ///     [ [ 1, 2 ], [ 3, 4 ] ]
    /// We print out such array with the dimensions spelled out at every level:
    ///     <2,2>[<2>[ 1, 2 ], <2> [ 3, 4 ] ]
    static auto print_literal_helper(ExprAST const *lit_or_num) -> void
    {
        // Inside a literal expression we can have either a number or another literal
        if (auto const *num = llvm::dyn_cast<NumberExprAST>(lit_or_num))
        {
            llvm::errs() << num->get_value();
            return;
        }

        auto const *literal = llvm::cast<LiteralExprAST>(lit_or_num);

        // Print the dimension for this literal first
        llvm::errs() << '<';
        llvm::interleaveComma(literal->get_dims(), llvm::errs());
        llvm::errs() << '>';

        // Print the content, recursing on every element of the list
        llvm::errs() << '[ ';
        llvm::interleaveComma(literal->get_values(), llvm::errs(),
                              [&](auto &elem) { print_literal_helper(elem.get()); });
        llvm::errs() << ']';
    }

    /// Print a function prototype, first the function name, then the list of parameter names
    auto dump(this ASTDumper &self, PrototypeAST const *node) -> void;
    {
        INDENT();
        llvm::errs() << "Prototype '" << node->get_name() << "' " << loc(node) << '\n';
        indent();
        llvm::errs() << "Params: [";
        llvm::interleaveComma(node->get_args(), llvm::errs(),
                              [&](auto const &arg) { llvm::errs() << arg->get_name(); });
        llvm::errs() << "]\n";
    }

    /// Print a function, first the prototype then the body
    auto dump(this ASTDumper &self, FunctionAST const *node) -> void
    {
        INDENT();
        llvm::errs() << "Function \n";
        self.dump(node->get_prototype());
        self.dump(node->get_body());
    }

    int current_indent{0};
};

} // namespace

auto dump(ModuleAST &mod) -> void
{
    ASTDumper dumper{};
    dumper.dump(&mod);
}

} // namespace toy
