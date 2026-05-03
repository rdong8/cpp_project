/// @module toyc.parser.ast

module;

#include <experimental/propagate_const>

#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringRef.h>

export module toyc.parser.ast;

import toyc.lexer;

import std;

export namespace toyc
{

using Dimension = std::int64_t;
using Shape = llvm::SmallVector<Dimension, 4>;

struct VarType
{
    Shape shape{};
};

class ExprAST
{
    using Self = ExprAST;

  public:
    enum class Kind : std::uint8_t
    {
        VarDecl,
        Return,
        Num,
        Literal,
        Var,
        BinOp,
        Call,
        Print
    };

    ExprAST(Kind kind, Location location)
        : kind{kind}
        , location{std::move(location)}
    {
    }

    virtual ~ExprAST() = default;

    [[nodiscard]]
    auto get_kind(this Self const &self) -> Kind
    {
        return self.kind;
    }

    [[nodiscard]]
    auto loc(this Self const &self) -> Location const &
    {
        return self.location;
    }

  private:
    Kind kind;
    Location location;
};

template <typename T> using ASTPtr = std::experimental::propagate_const<std::unique_ptr<T>>;

/// A block-list of expressions
using ExprASTList = std::vector<ASTPtr<ExprAST>>;

/// Expression class for numeric literals like "1.0"
class NumberExprAST final : public ExprAST
{
    using Self = NumberExprAST;

  public:
    [[nodiscard]]
    static auto classof(ExprAST const *c) -> bool
    {
        return c->get_kind() == Kind::Num;
    }

    NumberExprAST(Location location, Value value)
        : ExprAST{Kind::Num, std::move(location)}
        , value{value}
    {
    }

    [[nodiscard]]
    auto get_value(this Self const &self) -> Value
    {
        return self.value;
    }

  private:
    Value value;
};

/// Expression class for a literal value
class LiteralExprAST final : public ExprAST
{
    using Self = LiteralExprAST;

  public:
    using Values = std::vector<ASTPtr<ExprAST>>;

    [[nodiscard]]
    static auto classof(ExprAST const *c) -> bool
    {
        return c->get_kind() == Kind::Literal;
    }

    LiteralExprAST(Location location, Values values, Shape dims)
        : ExprAST{Kind::Literal, std::move(location)}
        , values{std::move(values)}
        , dims{std::move(dims)}
    {
    }

    [[nodiscard]]
    auto get_values(this Self const &self) -> llvm::ArrayRef<ASTPtr<ExprAST>>
    {
        return self.values;
    }

    [[nodiscard]]
    auto get_dims(this Self const &self) -> llvm::ArrayRef<Dimension>
    {
        return self.dims;
    }

  private:
    Values values;
    Shape dims;
};

/// Expression class for referencing a variable, like "a"
class VariableExprAST final : public ExprAST
{
    using Self = VariableExprAST;

  public:
    [[nodiscard]]
    static auto classof(ExprAST const *c) -> bool
    {
        return c->get_kind() == Kind::Var;
    }

    VariableExprAST(Location location, std::string name)
        : ExprAST{Kind::Var, std::move(location)}
        , name{std::move(name)}
    {
    }

    [[nodiscard]]
    auto get_name(this Self const &self) -> llvm::StringRef
    {
        return self.name;
    }

  private:
    std::string name;
};

/// Expression class for defining a variable
class VarDeclExprAST final : public ExprAST
{
    using Self = VarDeclExprAST;

  public:
    [[nodiscard]]
    static auto classof(ExprAST const *c) -> bool
    {
        return c->get_kind() == Kind::VarDecl;
    }

    VarDeclExprAST(Location location, std::string name, VarType type, ASTPtr<ExprAST> initial_value)
        : ExprAST{Kind::VarDecl, std::move(location)}
        , name{std::move(name)}
        , type{std::move(type)}
        , initial_value{std::move(initial_value)}
    {
    }

    [[nodiscard]]
    auto get_name(this Self const &self) -> llvm::StringRef
    {
        return self.name;
    }

    [[nodiscard]]
    auto get_initializer(this Self const &self) -> ExprAST const *
    {
        return self.initial_value.get();
    }

    [[nodiscard]]
    auto get_type(this Self const &self) -> VarType
    {
        return self.type;
    }

  private:
    std::string name;
    VarType type;
    ASTPtr<ExprAST> initial_value;
};

/// Expression class for a return operator
class ReturnExprAST final : public ExprAST
{
    using Self = ReturnExprAST;

  public:
    [[nodiscard]]
    static auto classof(ExprAST const *c) -> bool
    {
        return c->get_kind() == Kind::Return;
    }

    ReturnExprAST(Location location, ASTPtr<ExprAST> expr)
        : ExprAST{Kind::Return, std::move(location)}
        , expr{std::move(expr)}
    {
    }

    [[nodiscard]]
    auto get_expr(this Self const &self) -> ExprAST const *
    {
        return self.expr.get();
    }

  private:
    ASTPtr<ExprAST> expr;
};

/// Expression class for binary operator
class BinaryExprAST final : public ExprAST
{
    using Self = BinaryExprAST;

  public:
    [[nodiscard]]
    static auto classof(ExprAST const *c) -> bool
    {
        return c->get_kind() == Kind::BinOp;
    }

    BinaryExprAST(Location location, char op, ASTPtr<ExprAST> lhs, ASTPtr<ExprAST> rhs)
        : ExprAST{Kind::BinOp, std::move(location)}
        , op{op}
        , lhs{std::move(lhs)}
        , rhs{std::move(rhs)}
    {
    }

    [[nodiscard]]
    auto get_op(this Self const &self) -> char
    {
        return self.op;
    }

    [[nodiscard]]
    auto get_lhs(this Self const &self) -> ExprAST const *
    {
        return self.lhs.get();
    }

    [[nodiscard]]
    auto get_rhs(this Self const &self) -> ExprAST const *
    {
        return self.rhs.get();
    }

  private:
    char op;
    ASTPtr<ExprAST> lhs, rhs;
};

/// Expression class for function calls
class CallExprAST final : public ExprAST
{
    using Self = CallExprAST;

  public:
    using Args = std::vector<ASTPtr<ExprAST>>;

    [[nodiscard]]
    static auto classof(ExprAST const *c) -> bool
    {
        return c->get_kind() == Kind::Call;
    }

    CallExprAST(Location location, std::string callee, Args args)
        : ExprAST{Kind::Call, std::move(location)}
        , callee{std::move(callee)}
        , args{std::move(args)}
    {
    }

    [[nodiscard]]
    auto get_callee(this Self const &self) -> llvm::StringRef
    {
        return self.callee;
    }

    [[nodiscard]]
    auto get_args(this Self const &self) -> llvm::ArrayRef<ASTPtr<ExprAST>>
    {
        return self.args;
    }

  private:
    std::string callee;
    Args args;
};

/// Expression class for builtin print calls
class PrintExprAST final : public ExprAST
{
    using Self = PrintExprAST;

  public:
    [[nodiscard]]
    static auto classof(ExprAST const *c) -> bool
    {
        return c->get_kind() == Kind::Print;
    }

    PrintExprAST(Location location, ASTPtr<ExprAST> arg)
        : ExprAST{Kind::Print, std::move(location)}
        , arg{std::move(arg)}
    {
    }

    [[nodiscard]]
    auto get_arg(this Self const &self) -> ExprAST const *
    {
        return self.arg.get();
    }

  private:
    ASTPtr<ExprAST> arg;
};

/// This class represents the "prototype" of a fucntion, which captures its name and argument names (thus implicitly the
/// number of arguments the function takes)
class PrototypeAST final
{
    using Self = PrototypeAST;

  public:
    using Args = std::vector<ASTPtr<VariableExprAST>>;

    PrototypeAST(Location location, std::string name, Args args)
        : location{std::move(location)}
        , name{std::move(name)}
        , args{std::move(args)}
    {
    }

    [[nodiscard]]
    auto loc(this Self const &self) -> Location const &
    {
        return self.location;
    }

    [[nodiscard]]
    auto get_name(this Self const &self) -> llvm::StringRef
    {
        return self.name;
    }

    [[nodiscard]]
    auto get_args(this Self const &self) -> llvm::ArrayRef<ASTPtr<VariableExprAST>>
    {
        return self.args;
    }

  private:
    Location location;
    std::string name;
    Args args;
};

/// This class represents a function definition itself
class FunctionAST final
{
    using Self = FunctionAST;

  public:
    FunctionAST(ASTPtr<PrototypeAST> prototype, ASTPtr<ExprASTList> body)
        : prototype{std::move(prototype)}
        , body{std::move(body)}
    {
    }

    [[nodiscard]]
    auto get_prototype(this Self const &self) -> PrototypeAST const *
    {
        return self.prototype.get();
    }

    [[nodiscard]]
    auto get_body(this Self const &self) -> ExprASTList const *
    {
        return self.body.get();
    }

  private:
    ASTPtr<PrototypeAST> prototype;
    ASTPtr<ExprASTList> body;
};

/// This class represents the list of functions to be processed together
class ModuleAST final
{
    using Self = ModuleAST;

  public:
    ModuleAST(std::vector<FunctionAST> functions)
        : functions{std::move(functions)}
    {
    }

    [[nodiscard]]
    auto begin(this Self const &self) -> auto
    {
        return self.functions.begin();
    }

    [[nodiscard]]
    auto end(this Self const &self) -> auto
    {
        return self.functions.end();
    }

  private:
    std::vector<FunctionAST> functions;
};

auto dump(ModuleAST &) -> void;

} // namespace toyc
