module;

#include <llvm/ADT/STLExtras.h>
#include <llvm/ADT/Twine.h>
#include <llvm/ADT/TypeSwitch.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/FormatAdapters.h>
#include <llvm/Support/raw_ostream.h>
#include <magic_enum/magic_enum.hpp>

module toyc.parser.ast;

import std;

import toyc.lexer;
import toyc.utility;

namespace toyc
{

namespace
{

[[nodiscard]]
auto loc(auto const *node) -> auto
{
    auto const &loc{node->loc()};
    return llvm::Twine{'@'} + *loc.filename + ":" + llvm::Twine{loc.line} + ":" + llvm::Twine{loc.col};
}

/// Helper class that implements the AST tree traversal and prints the nodes along the way. The only data member is the
/// current indentation level.
class ASTDumper final
{
    using Self = ASTDumper;

  public:
    /// Print a module, prints the functions in sequence
    auto dump(this Self &self, ModuleAST const *node) -> void
    {
        auto indent{self.indent()};
        self.println("Module:");

        for (auto const &func : *node)
        {
            self.dump(&func);
        }
    }

  private:
    std::uint32_t current_indent{0};

    /// RAII helper to manage increasing/decreasing indentation as we traverse the AST
    class Indent final
    {
        using Self = Indent;

        std::uint32_t &level;

      public:
        Indent(std::uint32_t &level)
            : level{level}
        {
            ++this->level;
        }

        ~Indent()
        {
            --this->level;
        }
    };

    auto print_indent(this Self const &self) -> void
    {
        eprint("{}", llvm::fmt_repeat(' ', self.current_indent));
    }

    template <typename... Args> auto print(this Self const &self, std::string_view fmt, Args &&...args) -> void
    {
        self.print_indent();
        toyc::eprint(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args> auto println(this Self const &self, std::string_view fmt, Args &&...args) -> void
    {
        self.print_indent();
        toyc::eprintln(fmt, std::forward<Args>(args)...);
    }

    /// Increases indent in the current scope
    [[nodiscard]]
    auto indent(this Self &self) -> Indent
    {
        return Indent{self.current_indent};
    }

    /// Print type: only the shape is printed between the '<' and '>'
    auto dump(this Self const &, VarType const &type) -> void
    {
        toyc::eprint("<");
        llvm::interleaveComma(type.shape, llvm::errs());
        toyc::eprint(">");
    }

    /// Dispatch to generic expressions to the appropriate subclass using RTTI
    auto dump(this Self &self, ExprAST const *expr) -> void
    {
        llvm::TypeSwitch<ExprAST const *>(expr)
            .Case<BinaryExprAST, CallExprAST, LiteralExprAST, NumberExprAST, PrintExprAST, ReturnExprAST,
                  VarDeclExprAST, VariableExprAST>([&](auto const *node) { self.dump(node); })
            .Default(
                [&](ExprAST const *)
                {
                    auto indent{self.indent()};
                    self.println("<unknown Expr, kind {}>", magic_enum::enum_name(expr->get_kind()));
                });
    }

    /// A "block", or list of expressions
    auto dump(this Self &self, ExprASTList const *expr_list) -> void
    {
        auto indent{self.indent()};
        self.println("Block {{");

        for (auto const &expr : *expr_list)
        {
            self.dump(expr.get());
        }

        self.println("} // Block");
    }

    /// A literal number, just print the value
    auto dump(this Self &self, NumberExprAST const *num) -> void
    {
        auto indent{self.indent()};
        self.println("{} {}", num->get_value(), loc(num));
    }

    /// Helper to recursively print a literal. Handles nested arrays like:
    ///     [ [ 1, 2 ], [ 3, 4 ] ]
    /// We print out such array with the dimensions spelled out at every level:
    ///     <2,2>[<2>[ 1, 2 ], <2>[ 3, 4 ] ]
    static auto print_literal_helper(ExprAST const *lit_or_num) -> void
    {
        // Inside a literal expression we can have either a number or another literal
        if (auto const *num{llvm::dyn_cast<NumberExprAST>(lit_or_num)})
        {
            toyc::eprint("{0:e}", num->get_value());
            return;
        }

        auto const *literal{llvm::cast<LiteralExprAST>(lit_or_num)};

        // Print the dimension for this literal first
        toyc::eprint("<");
        llvm::interleaveComma(literal->get_dims(), llvm::errs());
        toyc::eprint(">[ ");

        // Print the content, recursing on every element of the list
        llvm::interleaveComma(literal->get_values(), llvm::errs(),
                              [&](auto &elem) { print_literal_helper(elem.get()); });
        toyc::eprint(" ]");
    }

    /// Print a literal, see above for implementation
    auto dump(this Self &self, LiteralExprAST const *node) -> void
    {
        auto indent{self.indent()};
        self.print("Literal: ");
        self.print_literal_helper(node);
        toyc::eprintln(" {}", loc(node));
    }

    /// Print a variable reference (just a name)
    auto dump(this Self &self, VariableExprAST const *node) -> void
    {
        auto indent{self.indent()};
        self.println("var: {} {}", node->get_name(), loc(node));
    }

    /// A variable declaration is printing the variable name, type and then recurse on the initializer
    auto dump(this Self &self, VarDeclExprAST const *var_decl) -> void
    {
        auto indent{self.indent()};
        self.print("VarDecl {}", var_decl->get_name());
        self.dump(var_decl->get_type());
        toyc::eprintln(" {}", loc(var_decl));
        self.dump(var_decl->get_initializer());
    }

    /// Print the return and its optional argument
    auto dump(this Self &self, ReturnExprAST const *node) -> void
    {
        auto indent{self.indent()};
        self.println("Return");

        if (auto const maybe_expr{node->get_expr()})
        {
            return self.dump(maybe_expr);
        }

        {
            auto indent{self.indent()};
            self.println("(void)");
        }
    }

    /// Print a binary operation, first the operator and then recurse into LHS and RHS
    auto dump(this Self &self, BinaryExprAST const *node) -> void
    {
        auto indent{self.indent()};
        self.println("BinOp: {} {}", node->get_op(), loc(node));
        self.dump(node->get_lhs());
        self.dump(node->get_rhs());
    }

    /// Print a call expression, first the callee name and then the list of args
    auto dump(this Self &self, CallExprAST const *node) -> void
    {
        auto indent{self.indent()};
        self.println("Call '{}' [ {}", node->get_callee(), loc(node));

        for (auto const &arg : node->get_args())
        {
            self.dump(arg.get());
        }

        self.println("]");
    }

    /// Print a builtin print call, first the builtin name and then the argument
    auto dump(this Self &self, PrintExprAST const *node) -> void
    {
        auto indent{self.indent()};
        self.println("Print [{}", loc(node));
        self.dump(node->get_arg());
        self.println("]");
    }

    /// Print a function prototype, first the function name, then the list of parameter names
    auto dump(this Self &self, PrototypeAST const *node) -> void
    {
        auto indent{self.indent()};
        self.println("Proto '{}' {}", node->get_name(), loc(node));
        self.print("Params: [");
        llvm::interleaveComma(node->get_args(), llvm::errs(),
                              [&](auto const &arg) { toyc::eprint("{}", arg->get_name()); });
        toyc::eprintln("]");
    }

    /// Print a function, first the prototype then the body
    auto dump(this Self &self, FunctionAST const *node) -> void
    {
        auto indent{self.indent()};
        self.println("Function ");
        self.dump(node->get_prototype());
        self.dump(node->get_body());
    }
};

} // namespace

auto dump(ModuleAST &mod) -> void
{
    ASTDumper dumper{};
    dumper.dump(&mod);
}

} // namespace toyc
