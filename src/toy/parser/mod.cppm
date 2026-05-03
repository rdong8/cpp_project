/// @module toyc.parser

module;

#include <llvm/ADT/STLExtras.h>
#include <llvm/ADT/StringExtras.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/raw_ostream.h>

export module toyc.parser;

import std;

export import toyc.parser.ast;

import toyc.lexer;
import toyc.utility;

export namespace toyc
{

/// Simple recursive descent parser. Produces a well-formed AST from a stream of `Token`'s supplied by the `Lexer`.
/// No semantic checks or symbol resolution is performed. For example, variables are referenced by string and the code
/// could refer to undeclared variables and still have parsing succeed.
class Parser final
{
    using Self = Parser;

  public:
    Parser(Lexer &lexer)
        : lexer{lexer}
    {
    }

    /// Parse a full module (list of function definitions)
    [[nodiscard]]
    auto parse_module(this Self &self) -> ASTPtr<ModuleAST>
    {
        std::ignore = self.lexer.get_next_token();

        // Parse functions one at a time and accumulate in a vector
        std::vector<FunctionAST> functions{};

        while (auto f{self.parse_definition()})
        {
            functions.push_back(std::move(*f));

            if (self.lexer.get_current_token() == Token::Eof)
            {
                break;
            }
        }

        // If we didn't reach EOF, there was an error during parsing
        if (self.lexer.get_current_token() != Token::Eof)
        {
            return self.parse_error<ModuleAST>("nothing", "at end of module");
        }

        return std::make_unique<ModuleAST>(std::move(functions));
    }

  private:
    Lexer &lexer;

    /// Helper function to signal errors while parsing
    /// @param[in] expected expected token
    /// @param[in] context more context
    template <typename R, typename T, typename U = std::string_view>
    [[nodiscard]]
    auto parse_error(this Self const &self, T &&expected, U &&context = "") -> ASTPtr<R>
    {
        auto cur_token{self.lexer.get_current_token()};
        auto const last_loc{self.lexer.get_last_location()};

        eprint("Parse error({}, {}): expected '{}' {} but got Token {}", last_loc.line, last_loc.col, expected, context,
               cur_token);

        if (std::isprint(std::to_underlying(cur_token)))
        {
            eprintln(" '{}'", static_cast<char>(cur_token));
        }

        eprintln("");

        return nullptr;
    }

    /// return ::= return ; | return expr ;
    [[nodiscard]]
    auto parse_return(this Self &self) -> ASTPtr<ReturnExprAST>
    {
        auto loc{self.lexer.get_last_location()};
        self.lexer.consume(Token::Return);

        // Return takes an optional argument
        ASTPtr<ExprAST> expr{};

        if (self.lexer.get_current_token() != Token::Semicolon)
        {
            expr = self.parse_expression();

            if (!expr)
            {
                return nullptr;
            }
        }

        return std::make_unique<ReturnExprAST>(std::move(loc), std::move(expr));
    }

    /// number_expr ::= number
    [[nodiscard]]
    auto parse_number_expr(this Self &self) -> ASTPtr<ExprAST>
    {
        auto loc{self.lexer.get_last_location()};
        auto result{std::make_unique<NumberExprAST>(std::move(loc), self.lexer.get_value())};
        self.lexer.consume(Token::Number);
        return std::move(result);
    }

    /// tensor_literal ::= [ literal_list ] | number
    /// literal_list ::= tensor_literal | tensor_literal, literal_list
    [[nodiscard]]
    auto parse_tensor_literal_expr(this Self &self) -> ASTPtr<ExprAST>
    {
        auto loc{self.lexer.get_last_location()};
        self.lexer.consume(Token::BracketOpen);

        // List of values at this nesting level
        LiteralExprAST::Values values{};

        while (true)
        {
            // We can have either another nested array or a number literal
            if (self.lexer.get_current_token() == Token::BracketOpen)
            {
                values.push_back(self.parse_tensor_literal_expr());

                if (!values.back())
                {
                    // Parse error in the nested array
                    return nullptr;
                }
            }
            else
            {
                if (self.lexer.get_current_token() != Token::Number)
                {
                    return self.parse_error<ExprAST>("<num> or [", "in literal expression");
                }

                values.push_back(self.parse_number_expr());
            }

            // End of this list on ']'
            if (self.lexer.get_current_token() == Token::BracketClose)
            {
                break;
            }

            // Elements are comma separated
            if (self.lexer.get_current_token() != Token{','})
            {
                return self.parse_error<ExprAST>("] or ,", "in literal expression");
            }

            // Eat `,`
            std::ignore = self.lexer.get_next_token();
        }

        if (values.empty())
        {
            return self.parse_error<ExprAST>("<something>", "to fill literal expression");
        }

        // Eat `]`
        std::ignore = self.lexer.get_next_token();

        // Dimensions for all nesting inside this level
        Shape dims{};

        // Next fill in the dimensions. First the current nesting level.
        dims.push_back(static_cast<Dimension>(values.size()));

        // If there's any nested array, process all of them and ensure dimensions are uniform.
        if (llvm::any_of(values, [](auto const &expr) { return llvm::isa<LiteralExprAST>(expr.get()); }))
        {
            auto const *const first_literal{llvm::dyn_cast<LiteralExprAST>(values.front().get())};

            if (!first_literal)
            {
                return self.parse_error<ExprAST>("uniform well-nested dimensions", "inside literal expression");
            }

            // Append the nested dimensions to the current level
            auto const first_dims{first_literal->get_dims()};
            dims.append(first_dims.begin(), first_dims.end());

            // Sanity check that shape is uniform across all elements of the list
            for (auto const &expr : values)
            {
                auto const *const expr_literal{llvm::cast<LiteralExprAST>(expr.get())};

                if (!expr_literal)
                {
                    return self.parse_error<ExprAST>("uniform well-nested dimensions", "inside literal expression");
                }

                if (expr_literal->get_dims() != first_dims)
                {
                    return self.parse_error<ExprAST>("uniform well-nested dimensions", "inside literal expression");
                }
            }
        }

        return std::make_unique<LiteralExprAST>(std::move(loc), std::move(values), std::move(dims));
    }

    /// paren_expr ::= '(' expression ')'
    [[nodiscard]]
    auto parse_paren_expr(this Self &self) -> ASTPtr<ExprAST>
    {
        // Eat `(`
        std::ignore = self.lexer.get_next_token();

        auto expr{self.parse_expression()};

        if (!expr)
        {
            return nullptr;
        }

        if (self.lexer.get_current_token() != Token::ParenthesesClose)
        {
            return self.parse_error<ExprAST>(")", "to close expression with parentheses");
        }

        self.lexer.consume(Token::ParenthesesClose);

        return expr;
    }

    /// identifier_expr
    ///     ::= identifier
    ///     ::: identifier '(' expression ')'
    [[nodiscard]]
    auto parse_identifier_expr(this Self &self) -> ASTPtr<ExprAST>
    {
        std::string name{self.lexer.get_identifier()};

        auto loc{self.lexer.get_last_location()};
        // Eat identifier
        std::ignore = self.lexer.get_next_token();

        // Simple variable reference
        if (self.lexer.get_current_token() != Token::ParenthesesOpen)
        {
            return std::make_unique<VariableExprAST>(std::move(loc), std::move(name));
        }

        // This is a function call
        self.lexer.consume(Token::ParenthesesOpen);
        CallExprAST::Args args{};

        if (self.lexer.get_current_token() != Token::ParenthesesClose)
        {
            while (true)
            {
                if (auto arg{self.parse_expression()})
                {
                    args.push_back(std::move(arg));
                }
                else
                {
                    return nullptr;
                }

                if (self.lexer.get_current_token() == Token::ParenthesesClose)
                {
                    break;
                }

                if (self.lexer.get_current_token() != Token{','})
                {
                    return self.parse_error<ExprAST>(", or )", "in argument list");
                }

                std::ignore = self.lexer.get_next_token();
            }
        }

        self.lexer.consume(Token::ParenthesesClose);

        // Can be a builtin call to `print`
        if (name == "print")
        {
            if (args.size() != 1)
            {
                return self.parse_error<ExprAST>("<single arg>", "as argument to `print()`");
            }

            return std::make_unique<PrintExprAST>(std::move(loc), std::move(args.front()));
        }

        // Call to a user-defined function
        return std::make_unique<CallExprAST>(std::move(loc), std::move(name), std::move(args));
    }

    /// primary
    ///     ::= identifier_expr
    ///     ::= number_expr
    ///     ::= paren_expr
    ///     ::= tensor_literal
    [[nodiscard]]
    auto parse_primary(this Self &self) -> ASTPtr<ExprAST>
    {
        switch (self.lexer.get_current_token())
        {
            case Token::Identifier:
            {
                return self.parse_identifier_expr();
            }
            case Token::Number:
            {
                return self.parse_number_expr();
            }
            case Token::ParenthesesOpen:
            {
                return self.parse_paren_expr();
            }
            case Token::BracketOpen:
            {
                return self.parse_tensor_literal_expr();
            }
            case Token::Semicolon:
                [[fallthrough]];
            case Token::BraceClose:
            {
                return nullptr;
            }
            default:
            {
                eprintln("Unknown token `{}` when expecting an expression", self.lexer.get_current_token());
                return nullptr;
            }
        }
    }

    /// Get the precedence of the pending binary operator token (1 is the lowest precedence)
    [[nodiscard]]
    auto get_token_precedence(this Self const &self) -> int
    {
        if (!::isascii(std::to_underlying(self.lexer.get_current_token())))
        {
            return -1;
        }

        switch (static_cast<char>(self.lexer.get_current_token()))
        {
            case '-':
                [[fallthrough]];
            case '+':
            {
                return 20;
            }
            case '*':
            {
                return 40;
            }
            default:
            {
                return -1;
            }
        }
    }

    /// Recursively parse the RHS of a binary expression
    /// @param[in] precedence the precedence of the current binary operator
    ///
    /// bin_op_rhs ::= ('+' primary)*
    [[nodiscard]]
    auto parse_bin_op_rhs(this Self &self, int precedence, ASTPtr<ExprAST> lhs) -> ASTPtr<ExprAST>
    {
        // If this is a binop, find its precednece
        while (true)
        {
            auto const token_precedence{self.get_token_precedence()};

            // If this is a binop that binds at least as tightly as the current binop, consume it
            // Otherwise we're done
            if (token_precedence < precedence)
            {
                return lhs;
            }

            // Ok, we know this is a binop
            auto const bin_op{self.lexer.get_current_token()};
            self.lexer.consume(bin_op);
            auto loc{self.lexer.get_last_location()};

            // Parse the primary expression after the binary operator
            auto rhs{self.parse_primary()};

            if (!rhs)
            {
                return self.parse_error<ExprAST>("expression", "to complete binary operator");
            }

            // If the binop binds less tightly with the RHS than the operator after `rhs`, let the pending operator take
            // `rhs` as its LHS
            auto const next_precedence{self.get_token_precedence()};

            if (token_precedence < next_precedence)
            {
                rhs = self.parse_bin_op_rhs(token_precedence + 1, std::move(rhs));

                if (!rhs)
                {
                    return nullptr;
                }
            }

            // Mergs LHS and RHS
            lhs = std::make_unique<BinaryExprAST>(std::move(loc), static_cast<char>(bin_op), std::move(lhs),
                                                  std::move(rhs));
        }
    }

    /// expression ::= primary binop rhs
    [[nodiscard]]
    auto parse_expression(this Self &self) -> ASTPtr<ExprAST>
    {
        auto lhs{self.parse_primary()};

        if (!lhs)
        {
            return nullptr;
        }

        return self.parse_bin_op_rhs(0, std::move(lhs));
    }

    /// type ::= < shape_list >
    /// shape_list ::= num | num , shape_list
    [[nodiscard]]
    auto parse_type(this Self &self) -> ASTPtr<VarType>
    {
        if (self.lexer.get_current_token() != Token{'<'})
        {
            return self.parse_error<VarType>("<", "to begin type");
        }

        // Eat `<`
        std::ignore = self.lexer.get_next_token();

        auto type{std::make_unique<VarType>()};

        while (self.lexer.get_current_token() == Token::Number)
        {
            type->shape.push_back(static_cast<Dimension>(self.lexer.get_value()));
            std::ignore = self.lexer.get_next_token();

            if (self.lexer.get_current_token() == Token{','})
            {
                std::ignore = self.lexer.get_next_token();
            }
        }

        if (self.lexer.get_current_token() != Token{'>'})
        {
            return self.parse_error<VarType>(">", "to end type");
        }

        // Eat `>`
        std::ignore = self.lexer.get_next_token();

        return type;
    }

    /// Parse a variable declaration, starting with a `var` keyword followed by an initializer and optional type (shape
    /// specification) before the initializer
    ///
    /// decl ::= var identifier [ type ] = expr
    [[nodiscard]]
    auto parse_declaration(this Self &self) -> ASTPtr<VarDeclExprAST>
    {
        if (self.lexer.get_current_token() != Token::Var)
        {
            return self.parse_error<VarDeclExprAST>("var", "to begin declaration");
        }

        auto loc{self.lexer.get_last_location()};
        // Eat `var`
        std::ignore = self.lexer.get_next_token();

        if (self.lexer.get_current_token() != Token::Identifier)
        {
            return self.parse_error<VarDeclExprAST>("identifier", "after `var` declaration");
        }

        std::string identifier{self.lexer.get_identifier()};
        // Eat identifier
        std::ignore = self.lexer.get_next_token();

        // Type is optioanl, it can be inferred
        ASTPtr<VarType> type{};

        if (self.lexer.get_current_token() == Token{'<'})
        {
            type = self.parse_type();

            if (!type)
            {
                return nullptr;
            }
        }
        else
        {
            type = std::make_unique<VarType>();
        }

        self.lexer.consume(Token{'='});

        auto expr{self.parse_expression()};

        if (!expr)
        {
            return nullptr;
        }

        return std::make_unique<VarDeclExprAST>( //
            std::move(loc),                      //
            std::move(identifier),               //
            std::move(*type),                    //
            std::move(expr)                      //
        );
    }

    /// Parse a block: a list of expressions separated by semicolons and wrapped in curly braces
    ///
    /// block ::= { expression_list }
    /// expression_list ::= blocK_expr ; expression_list
    /// block_expr ::= decl | "return" | expr
    [[nodiscard]]
    auto parse_block(this Self &self) -> ASTPtr<ExprASTList>
    {
        if (self.lexer.get_current_token() != Token::BraceOpen)
        {
            return self.parse_error<ExprASTList>("{", "to begin block");
        }

        self.lexer.consume(Token::BraceOpen);

        auto expr_list{std::make_unique<ExprASTList>()};

        // Ignore empty expressions: swallow sequences of semicolons
        while (self.lexer.get_current_token() == Token::Semicolon)
        {
            self.lexer.consume(Token::Semicolon);
        }

        while (self.lexer.get_current_token() != Token::BraceClose && self.lexer.get_current_token() != Token::Eof)
        {

            switch (self.lexer.get_current_token())
            {
                case Token::Var:
                {
                    // Variable declaration
                    expr_list->push_back(self.parse_declaration());
                    break;
                }
                case Token::Return:
                {
                    // Return statement
                    expr_list->push_back(self.parse_return());
                    break;
                }
                default:
                {
                    // General expression
                    expr_list->push_back(self.parse_expression());
                }
            }

            if (!expr_list->back())
            {
                return nullptr;
            }

            // Ensure that elements are separated by a semicolon
            if (self.lexer.get_current_token() != Token::Semicolon)
            {
                return self.parse_error<ExprASTList>(";", "after expression");
            }

            // Ignore empty expressions: swallow sequences of semicolons
            while (self.lexer.get_current_token() == Token::Semicolon)
            {
                self.lexer.consume(Token::Semicolon);
            }
        }

        if (self.lexer.get_current_token() != Token::BraceClose)
        {
            return self.parse_error<ExprASTList>("}", "to close block");
        }

        self.lexer.consume(Token::BraceClose);

        return expr_list;
    }

    /// prototype ::= def id '(' decl_list ')'
    /// decl_list ::= identifier | identifier , decl_list
    [[nodiscard]]
    auto parse_prototype(this Self &self) -> ASTPtr<PrototypeAST>
    {
        auto loc{self.lexer.get_last_location()};

        if (self.lexer.get_current_token() != Token::Def)
        {
            return self.parse_error<PrototypeAST>("def", "in prototype");
        }

        self.lexer.consume(Token::Def);

        if (self.lexer.get_current_token() != Token::Identifier)
        {
            return self.parse_error<PrototypeAST>("function name", "in prototype");
        }

        std::string function_name{self.lexer.get_identifier()};
        self.lexer.consume(Token::Identifier);

        if (self.lexer.get_current_token() != Token::ParenthesesOpen)
        {
            return self.parse_error<PrototypeAST>("(", "in prototype");
        }

        self.lexer.consume(Token::ParenthesesOpen);

        PrototypeAST::Args args{};

        if (self.lexer.get_current_token() != Token::ParenthesesClose)
        {
            while (true)
            {
                std::string name{self.lexer.get_identifier()};
                auto loc{self.lexer.get_last_location()};
                self.lexer.consume(Token::Identifier);
                args.push_back(std::make_unique<VariableExprAST>(std::move(loc), std::move(name)));

                if (self.lexer.get_current_token() != Token{','})
                {
                    break;
                }

                self.lexer.consume(Token{','});

                if (self.lexer.get_current_token() != Token::Identifier)
                {
                    return self.parse_error<PrototypeAST>("identifier", "after `,` in function parameter list");
                }
            }
        }

        if (self.lexer.get_current_token() != Token::ParenthesesClose)
        {
            return self.parse_error<PrototypeAST>(")", "to end function prototype");
        }

        // Success
        self.lexer.consume(Token::ParenthesesClose);

        return std::make_unique<PrototypeAST>(std::move(loc), std::move(function_name), std::move(args));
    }

    /// Parse a function definition, we expect a prototype initiated with the `def` keyword, followed by a block
    /// containing a list of expressions
    ///
    /// definition ::= prototype block
    [[nodiscard]]
    auto parse_definition(this Self &self) -> ASTPtr<FunctionAST>
    {
        auto prototype{self.parse_prototype()};

        if (!prototype)
        {
            return nullptr;
        }

        if (auto block{self.parse_block()})
        {
            return std::make_unique<FunctionAST>(std::move(prototype), std::move(block));
        }

        return nullptr;
    }
};

} // namespace toyc
