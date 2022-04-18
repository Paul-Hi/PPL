#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

// Unary Operators: ++ --
// Binary Operators: + - * / % = < > ! , ; ^
//                   && || & | == <= >= != << >> .
// Special Symbols: -> ( ) { } [ ] , ; . : :: ()

enum token_type : uint16_t
{
    undefined,
    identifier,             // type, name or keyword
    unit,                   // unit/void type
    integer_literal,        // integer literal z.B. 3
    floating_point_literal, // floating point literal z.B 9.5
    boolean_literal,        // boolean literal \in { true, false }
    string_literal,         // string literal z.B. "Hallo Welt"
    plus,                   // +
    increment,              // ++
    minus,                  // -
    decrement,              // --
    star,                   // *
    slash,                  // /
    percent,                // %
    assign,                 // =
    less_then,              // <
    greather_then,          // >
    exclamation_mark,       // !
    circumflex,             // ^
    logical_and,            // &&
    logical_or,             // ||
    ampersand,              // &
    pipe,                   // |
    equal,                  // ==
    less_then_or_equal,     // <=
    greather_then_or_equal, // >=
    not_equal,              // !=
    shift_left,             // <<
    shift_right,            // >>
    transmutation_arrow,    // ->
    l_parentheses,          // (
    r_parentheses,          // )
    l_brace,                // {
    r_brace,                // }
    l_bracket,              // [
    r_bracket,              // ]
    comma,                  // ,
    semicolon,              // ;
    colon,                  // :
    double_colon,           // ::
    point,                  // .
    comment,                // comment
};

static std::string token_type_to_string(token_type type)
{
    switch (type)
    {
    case token_type::undefined:
        return "undefined";
    case token_type::identifier:
        return "identifier";
    case token_type::unit:
        return "unit";
    case token_type::integer_literal:
        return "integer_literal";
    case token_type::floating_point_literal:
        return "floating_point_literal";
    case token_type::boolean_literal:
        return "boolean_literal";
    case token_type::string_literal:
        return "string_literal";
    case token_type::plus:
        return "plus";
    case token_type::increment:
        return "increment";
    case token_type::minus:
        return "minus";
    case token_type::decrement:
        return "decrement";
    case token_type::star:
        return "star";
    case token_type::slash:
        return "slash";
    case token_type::percent:
        return "percent";
    case token_type::assign:
        return "assign";
    case token_type::less_then:
        return "less_then";
    case token_type::greather_then:
        return "greather_then";
    case token_type::exclamation_mark:
        return "exclamation_mark";
    case token_type::circumflex:
        return "circumflex";
    case token_type::logical_and:
        return "logical_and";
    case token_type::logical_or:
        return "logical_or";
    case token_type::ampersand:
        return "ampersand";
    case token_type::pipe:
        return "pipe";
    case token_type::equal:
        return "equal";
    case token_type::less_then_or_equal:
        return "less_then_or_equal";
    case token_type::greather_then_or_equal:
        return "greather_then_or_equal";
    case token_type::not_equal:
        return "not_equal";
    case token_type::shift_left:
        return "shift_left";
    case token_type::shift_right:
        return "shift_right";
    case token_type::transmutation_arrow:
        return "transmutation_arrow";
    case token_type::l_parentheses:
        return "l_parentheses";
    case token_type::r_parentheses:
        return "r_parentheses";
    case token_type::l_brace:
        return "l_brace";
    case token_type::r_brace:
        return "r_brace";
    case token_type::l_bracket:
        return "l_bracket";
    case token_type::r_bracket:
        return "r_bracket";
    case token_type::comma:
        return "comma";
    case token_type::semicolon:
        return "semicolon";
    case token_type::colon:
        return "colon";
    case token_type::double_colon:
        return "double_colon";
    case token_type::point:
        return "point";
    case token_type::comment:
        return "comment";
    }
}

struct token
{
    token_type type = token_type::undefined;
    std::string text;
    int32_t line          = 0;
    int32_t inline_offset = 0;
};

#endif TOKEN_HPP
