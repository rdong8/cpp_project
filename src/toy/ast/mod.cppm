module;

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Casting.h"

#include <experimental/propagate_const>

export module toyc.ast;

import toyc.lexer;

import std;

export namespace toy
{

using Dimension = std::int64_t;
using Shape = std::vector<Dimension>;

struct VarType
{
    Shape shape{};
};

class ExprAST
{
  public:
    enum class Kind
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

    ExprAST(Kind kind, Location location) : kind{kind}, location{std::move(location)} {}

    virtual ~ExprAST() = default;

    [[nodiscard]]
    auto get_kind(this ExprAST const &self) -> Kind
    {
        return self.kind;
    }

    [[nodiscard]]
    auto loc(this ExprAST const &self) -> Location const &
    {
        return self.location;
    }

  private:
    Kind kind{};
    Location location{};
};

/// A block-list of expressions
using ExprASTList = std::vector<std::experimental::propagate_const<std::unique_ptr<ExprAST>>>;

/// Expression class for numeric literals like "1.0"
class NumberExprAST : public ExprAST
{
  public:
    [[nodiscard]]
    static auto classof(ExprAST const *c) -> bool
    {
        return c->get_kind() == Kind::Num;
    }

    NumberExprAST(Location location, Value value) : ExprAST{Kind::Num, std::move(location)}, value{value} {}

    [[nodiscard]]
    auto get_value(this NumberExprAST const &self) -> Value
    {
        return self.value;
    }

  private:
    Value value{};
};

/// Expression class for a literal value
class LiteralExprAST : public ExprAST
{
  public:
    using Values = std::vector<std::experimental::propagate_const<std::unique_ptr<ExprAST>>>;

    [[nodiscard]]
    static auto classof(ExprAST const *c) -> bool
    {
        return c->get_kind() == Kind::Literal;
    }

    LiteralExprAST(Location location, Values values, Shape dims)
        : ExprAST{Kind::Literal, std::move(location)}, values{std::move(values)}, dims{std::move(dims)}
    {
    }

    template <typename Self>
    [[nodiscard]]
    auto get_values(this Self &&self) -> decltype(auto)
    {
        return llvm::ArrayRef{std::forward<Self>(self).values};
    }

    template <typename Self>
    [[nodiscard]]
    auto get_dims(this Self &&self) -> decltype(auto)
    {
        return llvm::ArrayRef{std::forward<Self>(self).dims};
    }

  private:
    Values values{};
    Shape dims{};
};

/// Expression class for referencing a variable, like "a"
class VariableExprAST : public ExprAST
{
  public:
    [[nodiscard]]
    static auto classof(ExprAST const *c) -> bool
    {
        return c->get_kind() == Kind::Var;
    }

    VariableExprAST(Location location, llvm::StringRef name) : ExprAST{Kind::Var, std::move(location)}, name{name} {}

    [[nodiscard]]
    auto get_name(this VariableExprAST const &self) -> llvm::StringRef
    {
        return self.name;
    }

  private:
    std::string name{};
};

/// Expression class for defining a variable
class VarDeclExprAST : public ExprAST
{
  public:
    [[nodiscard]]
    auto classof(ExprAST const *c) -> bool
    {
        return c->get_kind() == Kind::VarDecl;
    }

    VarDeclExprAST(Location location, std::string name, VarType type, std::unique_ptr<ExprAST> initial_value)
        : ExprAST{Kind::VarDecl, std::move(location)}, name{std::move(name)}, type{type},
          initial_value{std::move(initial_value)}
    {
    }

    [[nodiscard]]
    auto get_name(this VarDeclExprAST const &self) -> llvm::StringRef
    {
        return self.name;
    }

    template <typename Self>
    [[nodiscard]]
    auto get_initializer(this Self &&self) -> decltype(auto)
    {
        return std::forward<Self>(self).initial_value.get();
    }

    [[nodiscard]]
    auto get_type(this VarDeclExprAST const &self) -> VarType
    {
        return self.type;
    }

  private:
    std::string name{};
    VarType type{};
    std::experimental::propagate_const<std::unique_ptr<ExprAST>> initial_value{};
};

/// Expression class for a return operator
class ReturnExprAST : public ExprAST
{
  public:
    [[nodiscard]]
    static auto classof(ExprAST const *c) -> bool
    {
        return c->get_kind() == Kind::Return;
    }

    ReturnExprAST(Location location, std::unique_ptr<ExprAST> expr)
        : ExprAST{Kind::Return, std::move(location)}, expr{std::move(expr)}
    {
    }

    template <typename Self>
    [[nodiscard]]
    auto get_expr(this Self &&self) -> decltype(auto)
    {
        if (self.expr)
        {
            return std::make_optional(std::forward<Self>(self).expr.get());
        }

        return std::nullopt;
    }

  private:
    std::experimental::propagate_const<std::unique_ptr<ExprAST>> expr{};
};

/// Expression class for binary operator
class BinaryExprAST : public ExprAST
{
  public:
    [[nodiscard]]
    static auto classof(ExprAST const *c) -> bool
    {
        return c->get_kind() == Kind::BinOp;
    }

    BinaryExprAST(Location location, char op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs)
        : ExprAST{Kind::BinOp, std::move(location)}, op{op}, lhs{std::move(lhs)}, rhs{std::move(rhs)}
    {
    }

    [[nodiscard]]
    auto get_op(this BinaryExprAST const &self) -> char
    {
        return self.op;
    }

    template <typename Self>
    [[nodiscard]]
    auto get_lhs(this Self &&self) -> decltype(auto)
    {
        return std::forward<Self>(self).lhs.get();
    }

    template <typename Self>
    [[nodiscard]]
    auto get_rhs(this Self &&self) -> decltype(auto)
    {
        return std::forward<Self>(self).rhs.get();
    }

  private:
    char op{};
    std::experimental::propagate_const<std::unique_ptr<ExprAST>> lhs{}, rhs{};
};

/// Expression class for function calls
class CallExprAST : public ExprAST
{
  public:
    using Args = std::vector<std::experimental::propagate_const<std::unique_ptr<ExprAST>>>;

    [[nodiscard]]
    static auto classof(ExprAST const *c) -> bool
    {
        return c->get_kind() == Kind::Call;
    }

    CallExprAST(Location location, std::string callee, Args args)
        : ExprAST{Kind::Call, std::move(location)}, callee{std::move(callee)}, args{std::move(args)}
    {
    }

    [[nodiscard]]
    auto get_callee(this CallExprAST const &self) -> llvm::StringRef
    {
        return self.callee;
    }

    template <typename Self>
    [[nodiscard]]
    auto get_args(this Self &&self) -> decltype(auto)
    {
        return llvm::ArrayRef{std::forward<Self>(self).args};
    }

  private:
    std::string callee{};
    Args args{};
};

/// Expression class for builtin print calls
class PrintExprAST : public ExprAST
{
  public:
    [[nodiscard]]
    static auto classof(ExprAST const *c) -> bool
    {
        return c->get_kind() == Kind::Print;
    }

    PrintExprAST(Location location, std::unique_ptr<ExprAST> arg)
        : ExprAST{Kind::Print, std::move(location)}, arg{std::move(arg)}
    {
    }

    template <typename Self>
    [[nodiscard]]
    auto get_arg(this Self &&self) -> decltype(auto)
    {
        return std::forward<Self>(self).arg.get();
    }

  private:
    std::experimental::propagate_const<std::unique_ptr<ExprAST>> arg{};
};

/// This class represents the "prototype" of a fucntion, which captures its name and argument names (thus implicitly the
/// number of arguments the function takes)
class PrototypeAST
{
  public:
    using Args = std::vector<std::experimental::propagate_const<std::unique_ptr<VariableExprAST>>>;

    PrototypeAST(Location location, std::string name, Args args)
        : location{std::move(location)}, name{std::move(name)}, args{std::move(args)}
    {
    }

    [[nodiscard]]
    auto loc(this PrototypeAST const &self) -> Location const &
    {
        return self.location;
    }

    [[nodiscard]]
    auto get_name(this PrototypeAST const &self) -> llvm::StringRef
    {
        return self.name;
    }

    template <typename Self>
    [[nodiscard]]
    auto get_args(this Self &&self) -> decltype(auto)
    {
        return std::forward<Self>(self).args;
    }

  private:
    Location location{};
    std::string name{};
    Args args{};
};

/// This class represents a function definition itself
class FunctionAST
{
  public:
    FunctionAST(std::unique_ptr<PrototypeAST> prototype, std::unique_ptr<ExprASTList> body)
        : prototype{std::move(prototype)}, body{std::move(body)}
    {
    }

    template <typename Self>
    [[nodiscard]]
    auto get_prototype(this Self &&self) -> decltype(auto)
    {
        return std::forward<Self>(self).prototype.get();
    }

    template <typename Self>
    [[nodiscard]]
    auto get_body(this Self &&self) -> decltype(auto)
    {
        return std::forward<Self>(self).body.get();
    }

  private:
    std::experimental::propagate_const<std::unique_ptr<PrototypeAST>> prototype{};
    std::experimental::propagate_const<std::unique_ptr<ExprASTList>> body{};
};

/// This class represents the list of functions to be processed together
class ModuleAST
{
  public:
    ModuleAST(std::vector<FunctionAST> functions) : functions{std::move(functions)} {}

    template <typename Self>
    [[nodiscard]]
    auto begin(this Self &&self) -> decltype(auto)
    {
        return std::forward<Self>(self).functions.begin();
    }

    template <typename Self>
    [[nodiscard]]
    auto end(this Self &&self) -> decltype(auto)
    {
        return std::forward<Self>(self).functions.end();
    }

  private:
    std::vector<FunctionAST> functions{};
};

auto dump(ModuleAST &) -> void;

} // namespace toy
