module;

#include "macros.hpp"

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/FormatProviders.h"
#include "llvm/Support/FormatVariadic.h"
#include "llvm/Support/raw_ostream.h"

export module toyc:lexer;

import std;

export namespace toy
{

using Value = double;
using Character = int;
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
    semicolon = ';',
    parenthese_open = '(',
    parenthese_close = ')',
    brace_open = '{',
    brace_close = '}',
    bracket_open = '[',
    bracket_close = ']',

    eof = -1,

    // Commands
    return_ = -2,
    var = -3,
    def = -4,

    // Primary
    identifier = -5,
    number = -6,
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
        case toy::Token::semicolon:
            [[fallthrough]];
        case toy::Token::parenthese_open:
            [[fallthrough]];
        case toy::Token::parenthese_close:
            [[fallthrough]];
        case toy::Token::brace_open:
            [[fallthrough]];
        case toy::Token::brace_close:
            [[fallthrough]];
        case toy::Token::bracket_open:
            [[fallthrough]];
        case toy::Token::bracket_close:
        {
            return format_provider<char>::format(static_cast<char>(token), os, style);
        }
        case toy::Token::eof:
        {
            return format_provider<StringRef>::format("EOF", os, style);
        }
        case toy::Token::return_:
        {
            return format_provider<StringRef>::format("return", os, style);
        }
        case toy::Token::var:
        {
            return format_provider<StringRef>::format("var", os, style);
        }
        case toy::Token::def:
        {
            return format_provider<StringRef>::format("def", os, style);
        }
        case toy::Token::identifier:
        {
            return format_provider<StringRef>::format("identifier", os, style);
        }
        case toy::Token::number:
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
    [[nodiscard]] auto get_current_token() const -> Token { return current_token; }

    /// Move to the next token in the stream and return it
    [[nodiscard]] auto get_next_token() -> Token { return current_token = get_token(); }

    /// Move to the next token in the stream, asserting ont eh current token matching the expectation
    auto consume(Token token) -> void
    {
        TOY_ASSERT(token == current_token, "Token " << token << " doesn't match current_token " << current_token);
        std::ignore = get_next_token();
    }

    /// Return the current identifier (prequisite: get_current_token() == token_identifier)
    [[nodiscard]] auto get_identifier() const -> llvm::StringRef
    {
        TOY_ASSERT(current_token == Token::identifier,
                   "Expected Token::identifier, got current_token = " << current_token);
        return identifier_str;
    }

    /// Return the current number (prequisite: get_current_token() == token_number)
    [[nodiscard]] auto get_value() const -> Value
    {
        TOY_ASSERT(current_token == Token::number, "Expected Token::number, got current_token = " << current_token);
        return number_value;
    }

    /// Return the location for the beginning of the current token
    [[nodiscard]] auto get_last_location() const -> Location { return last_location; }

    /// Return the current line in the file
    [[nodiscard]] auto get_line() -> Position { return current_line_number; }

    /// Return the current column in the file
    [[nodiscard]] auto get_column() -> Position { return current_column_number; }

  private:
    /// Delegate to a derived class fetching the next line. Returns an empty string to signal EOF. Lines are always
    /// expected to finish with '\n'.
    [[nodiscard]] virtual auto read_next_line() -> llvm::StringRef = 0;

    /// Return the last character read from the stream as a Character
    [[nodiscard]] auto get_last_char() const -> Character { return static_cast<Character>(last_char); }

    /// Return the next character from the stream. This manages the buffer for the current line and requests the
    /// next line buffer to the derived class as needed.
    [[nodiscard]] auto get_next_char() -> Character
    {
        // The current line buffer should not be empty unless it's the EOF
        if (current_line_buffer.empty())
        {
            return EOF;
        }

        ++current_column_number;

        auto const next_char = current_line_buffer.front();
        current_line_buffer = current_line_buffer.drop_front();

        if (current_line_buffer.empty())
        {
            current_line_buffer = read_next_line();
        }

        if (next_char == '\n')
        {
            ++current_line_number;
            current_column_number = 0;
        }

        return next_char;
    }

    /// Return the next token from stdin
    [[nodiscard]] auto get_token() -> Token
    {
        // Skip any whitespace
        while (std::isspace(get_last_char()))
        {
            last_char = Token{get_next_char()};
        }

        // Save the current location before reading the token characters
        last_location.line = current_line_number;
        last_location.col = current_column_number;

        // Identifier: [a-zA-Z][a-zA-Z0-9_]*
        if (std::isalpha(get_last_char()))
        {
            identifier_str = static_cast<char>(get_last_char());

            while (std::isalnum(static_cast<Character>(last_char = Token{get_next_char()})) || get_last_char() == '_')
            {
                identifier_str += static_cast<char>(last_char);
            }

            if (identifier_str == "return")
            {
                return Token::return_;
            }

            if (identifier_str == "def")
            {
                return Token::def;
            }

            if (identifier_str == "var")
            {
                return Token::var;
            }

            return Token::identifier;
        }

        // Number: [0-9.]+
        if (std::isdigit(get_last_char()) || get_last_char() == '.')
        {
            std::string number_str{};

            do
            {
                number_str += static_cast<char>(last_char);
                last_char = Token{get_next_char()};
            } while (std::isdigit(get_last_char()) || get_last_char() == '.');

            auto const result = std::from_chars(number_str.data(), number_str.data() + number_str.size(), number_value);
            TOY_ASSERT(result.ec == std::errc{},
                       "from_chars failed on " << number_str << " after parsing " << (result.ptr - number_str.data())
                                               << " characters: got error code " << std::to_underlying(result.ec));

            return Token::number;
        }

        // Comment until end of line
        if (get_last_char() == '#')
        {
            do
            {
                last_char = Token{get_next_char()};
            } while (last_char != Token::eof && get_last_char() != '\n' && get_last_char() != '\r');

            if (last_char != Token::eof)
            {
                return get_token();
            }
        }

        // Check for end of file. Don't eat EOF.
        if (last_char == Token::eof)
        {
            return Token::eof;
        }

        // Otherwise, just return the character as its ASCII value
        Token const this_char{last_char};
        last_char = Token{get_next_char()};
        return this_char;
    }

    /// The last token read from the input
    Token current_token{Token::eof};

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

/// A lexer implementatino operating on an in-memory buffer
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

        while (current <= end && *current != '\0' && *current != '\n')
        {
            ++current;
        }

        if (current <= end && *current != '\0')
        {
            ++current;
        }

        return llvm::StringRef{begin, static_cast<std::size_t>(current - begin)};
    }

    char const *current{}, *end{};
};

} // namespace toy
