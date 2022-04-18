#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

// Unary Operators: ++ --
// Binary Operators: + - * / % = < > ! , ; && || == <= >= !=.
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
    logical_and,            // &&
    logical_or,             // ||
    equal,                  // ==
    less_then_or_equal,     // <=
    greather_then_or_equal, // >=
    not_equal,              // !=
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
    // keyword tokens
    type_i32,        // i32
    type_f32,        // f32
    type_bool,       // bool
    type_str,        // str
    keyword_true,    // true
    keyword_false,   // false
    keyword_as,      // as
    keyword_if,      // if
    keyword_else,    // else
    keyword_while,   // while
    keyword_return,  // return
    keyword_pub,     // pub
    keyword_static,  // static
    keyword_type,    // type
    keyword_extends, // extends
    function_dump,   // dump
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
    case token_type::logical_and:
        return "logical_and";
    case token_type::logical_or:
        return "logical_or";
    case token_type::equal:
        return "equal";
    case token_type::less_then_or_equal:
        return "less_then_or_equal";
    case token_type::greather_then_or_equal:
        return "greather_then_or_equal";
    case token_type::not_equal:
        return "not_equal";
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
    case token_type::type_i32:
        return "type_i32";
    case token_type::type_f32:
        return "type_f32";
    case token_type::type_bool:
        return "type_bool";
    case token_type::type_str:
        return "type_str";
    case token_type::keyword_as:
        return "keyword_as";
    case token_type::keyword_true:
        return "keyword_true";
    case token_type::keyword_false:
        return "keyword_false";
    case token_type::keyword_if:
        return "keyword_if";
    case token_type::keyword_else:
        return "keyword_else";
    case token_type::keyword_while:
        return "keyword_while";
    case token_type::keyword_return:
        return "keyword_return";
    case token_type::keyword_pub:
        return "keyword_pub";
    case token_type::keyword_static:
        return "keyword_static";
    case token_type::keyword_type:
        return "keyword_type";
    case token_type::keyword_extends:
        return "keyword_extends";
    case token_type::function_dump:
        return "function_dump";
    default:
        return "undefined";
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
