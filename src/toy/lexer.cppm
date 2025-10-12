module;

#include "macros.hpp"

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/FormatProviders.h"
#include "llvm/Support/FormatVariadic.h"
#include "llvm/Support/raw_ostream.h"

export module toyc.lexer;

import std;

export namespace toy
{

// TODO: use std::float64_t
using Value = double;
using Character = std::int32_t;
using Position = std::int32_t;

/// A location in a file
struct Location
{
    std::shared_ptr<std::string> filename{};
    Position line{};
    Position col{};
};

enum class Token : Character
{
    Semicolon = ';',
    ParenthesesOpen = '(',
    ParenthesesClose = ')',
    BraceOpen = '{',
    BraceClose = '}',
    BracketOpen = '[',
    BracketClose = ']',

    Eof = -1,

    // Commands
    Return = -2,
    Var = -3,
    Def = -4,

    // Primary
    Identifier = -5,
    Number = -6,
};

} // namespace toy

export namespace llvm
{

template <> struct format_provider<toy::Token>
{
    static auto format(toy::Token token, llvm::raw_ostream &os, llvm::StringRef style) -> void
    {
        switch (token)
        {
        case toy::Token::Semicolon:
            [[fallthrough]];
        case toy::Token::ParenthesesOpen:
            [[fallthrough]];
        case toy::Token::ParenthesesClose:
            [[fallthrough]];
        case toy::Token::BraceOpen:
            [[fallthrough]];
        case toy::Token::BraceClose:
            [[fallthrough]];
        case toy::Token::BracketOpen:
            [[fallthrough]];
        case toy::Token::BracketClose:
        {
            return format_provider<char>::format(static_cast<char>(token), os, style);
        }
        case toy::Token::Eof:
        {
            return format_provider<StringRef>::format("EOF", os, style);
        }
        case toy::Token::Return:
        {
            return format_provider<StringRef>::format("return", os, style);
        }
        case toy::Token::Var:
        {
            return format_provider<StringRef>::format("var", os, style);
        }
        case toy::Token::Def:
        {
            return format_provider<StringRef>::format("def", os, style);
        }
        case toy::Token::Identifier:
        {
            return format_provider<StringRef>::format("identifier", os, style);
        }
        case toy::Token::Number:
        {
            return format_provider<StringRef>::format("number", os, style);
        }
        }

        return format_provider<StringRef>::format(
            formatv("{0} ({1})", std::to_underlying(token), static_cast<char>(token)).str(), os, style);
    }
};

auto operator<<(llvm::raw_ostream &os, toy::Token token) -> llvm::raw_ostream &
{
    format_provider<toy::Token>::format(token, os, {});
    return os;
}

} // namespace llvm

export namespace toy
{

/// The Lexer is an abstract base class providing all the facilities that the Parser expects. It goes through the
/// stream one token at a time and keeps track of the location in the file for debugging purposes. It relies on a
/// subclass to provide a `read_next_line()` method. The subclass can proceed by reading the next line from the
/// standard input or from a mmapped file.
class Lexer
{
  public:
    /// Create a lexer with the given filename. The filename is kept only for debugging purposes (attaching a
    /// Location to a Token).
    Lexer(std::string filename)
        : last_location{.filename = std::make_shared<std::string>(std::move(filename)), .line = 0, .col = 0}
    {
    }

    virtual ~Lexer() = default;

    /// Look at the current token in the stream
    [[nodiscard]] auto get_current_token(this Lexer const &self) -> Token { return self.current_token; }

    /// Move to the next token in the stream and return it
    [[nodiscard]] auto get_next_token(this Lexer &self) -> Token { return self.current_token = self.get_token(); }

    /// Move to the next token in the stream, asserting ont eh current token matching the expectation
    auto consume(this Lexer &self, Token token) -> void
    {
        TOY_ASSERT(token == self.current_token,
                   "Token " << token << " doesn't match current_token " << self.current_token);
        std::ignore = self.get_next_token();
    }

    /// Return the current identifier (prequisite: get_current_token() == token_identifier)
    [[nodiscard]] auto get_identifier(this Lexer const &self) -> llvm::StringRef
    {
        TOY_ASSERT(self.current_token == Token::Identifier,
                   "Expected Token::identifier, got current_token = " << self.current_token);
        return self.identifier_str;
    }

    /// Return the current number (prequisite: get_current_token() == token_number)
    [[nodiscard]] auto get_value(this Lexer const &self) -> Value
    {
        TOY_ASSERT(self.current_token == Token::Number,
                   "Expected Token::number, got current_token = " << self.current_token);
        return self.number_value;
    }

    /// Return the location for the beginning of the current token
    [[nodiscard]] auto get_last_location(this Lexer const &self) -> Location { return self.last_location; }

    /// Return the current line in the file
    [[nodiscard]] auto get_line(this Lexer const &self) -> Position { return self.current_line_number; }

    /// Return the current column in the file
    [[nodiscard]] auto get_column(this Lexer const &self) -> Position { return self.current_column_number; }

  private:
    /// Delegate to a derived class fetching the next line. Returns an empty string to signal EOF. Lines are always
    /// expected to finish with '\n'.
    [[nodiscard]] virtual auto read_next_line() -> llvm::StringRef = 0;

    /// Return the last character read from the stream as a Character
    [[nodiscard]] auto get_last_char(this Lexer const &self) -> Character
    {
        return static_cast<Character>(self.last_char);
    }

    /// Return the next character from the stream. This manages the buffer for the current line and requests the
    /// next line buffer to the derived class as needed.
    [[nodiscard]] auto get_next_char(this Lexer &self) -> Character
    {
        // The current line buffer should not be empty unless it's the EOF
        if (self.current_line_buffer.empty())
        {
            return EOF;
        }

        ++self.current_column_number;

        auto const next_char = self.current_line_buffer.front();
        self.current_line_buffer = self.current_line_buffer.drop_front();

        if (self.current_line_buffer.empty())
        {
            self.current_line_buffer = self.read_next_line();
        }

        if (next_char == '\n')
        {
            ++self.current_line_number;
            self.current_column_number = 0;
        }

        return next_char;
    }

    /// Return the next token from stdin
    [[nodiscard]] auto get_token(this Lexer &self) -> Token
    {
        // Skip any whitespace
        while (std::isspace(self.get_last_char()))
        {
            self.last_char = Token{self.get_next_char()};
        }

        // Save the current location before reading the token characters
        self.last_location.line = self.current_line_number;
        self.last_location.col = self.current_column_number;

        // Identifier: [a-zA-Z][a-zA-Z0-9_]*
        if (std::isalpha(self.get_last_char()))
        {
            self.identifier_str = static_cast<char>(self.get_last_char());

            while (std::isalnum(static_cast<Character>(self.last_char = Token{self.get_next_char()})) ||
                   self.get_last_char() == '_')
            {
                self.identifier_str += static_cast<char>(self.last_char);
            }

            if (self.identifier_str == "return")
            {
                return Token::Return;
            }

            if (self.identifier_str == "def")
            {
                return Token::Def;
            }

            if (self.identifier_str == "var")
            {
                return Token::Var;
            }

            return Token::Identifier;
        }

        // Number: [0-9.]+
        if (std::isdigit(self.get_last_char()) || self.get_last_char() == '.')
        {
            std::string number_str{};

            do
            {
                number_str += static_cast<char>(self.last_char);
                self.last_char = Token{self.get_next_char()};
            } while (std::isdigit(self.get_last_char()) || self.get_last_char() == '.');

            auto const result =
                std::from_chars(number_str.data(), number_str.data() + number_str.size(), self.number_value);
            TOY_ASSERT(result.ec == std::errc{},
                       "from_chars failed on " << number_str << " after parsing " << (result.ptr - number_str.data())
                                               << " characters: got error code " << std::to_underlying(result.ec));

            return Token::Number;
        }

        // Comment until end of line
        if (self.get_last_char() == '#')
        {
            do
            {
                self.last_char = Token{self.get_next_char()};
            } while (self.last_char != Token::Eof && self.get_last_char() != '\n' && self.get_last_char() != '\r');

            if (self.last_char != Token::Eof)
            {
                return self.get_token();
            }
        }

        // Check for end of file. Don't eat EOF.
        if (self.last_char == Token::Eof)
        {
            return Token::Eof;
        }

        // Otherwise, just return the character as its ASCII value
        auto const this_char{self.last_char};
        self.last_char = Token{self.get_next_char()};
        return this_char;
    }

    /// The last token read from the input
    Token current_token{Token::Eof};

    /// Location for `current_token`
    Location last_location{};

    /// If current_token is an identifier, this string contains its value
    std::string identifier_str{};

    /// If the current Token is a number, this contains the value
    Value number_value{};

    /// The last value returned by get_next_char(). We need to keep it around as we always need to read ahead 1
    /// character to decide when to end a token and we can't put it back in the stream after reading from it.
    Token last_char{' '};

    /// Keep track of the current line number in the input stream
    Position current_line_number{0};

    /// Keep track of the current column number in the input stream
    Position current_column_number{0};

    /// Buffer supplied by the derived class on class to `read_next_line()`
    llvm::StringRef current_line_buffer{"\n"};
};

/// A lexer implementation operating on an in-memory buffer
class LexerBuffer final : public Lexer
{
  public:
    LexerBuffer(char const *begin, char const *end, std::string filename)
        : Lexer{std::move(filename)}, current{begin}, end{end}
    {
    }

  private:
    /// Provide 1 line at a time to the Lexer, return an empty string when reaching the end of buffer
    [[nodiscard]] auto read_next_line() -> llvm::StringRef override
    {
        auto *begin = current;

        while (this->current <= this->end && *this->current != '\0' && *this->current != '\n')
        {
            ++this->current;
        }

        if (this->current <= this->end && *this->current != '\0')
        {
            ++this->current;
        }

        return llvm::StringRef{begin, static_cast<std::size_t>(this->current - begin)};
    }

    char const *current{}, *end{};
};

} // namespace toy
