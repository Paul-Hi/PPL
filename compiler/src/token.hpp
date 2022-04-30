#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <iostream>
#include <string>

// Unary Operators: ++ -- ! - +
// Binary Operators: + - * / % = < > , ; && || == <= >= !=.
// Special Symbols: -> ( ) { } [ ] , ; . : :: ()

#define TOKEN_TYPE_ENUMERATION(op)                                                                                                                                                                     \
    op(undefined) op(token_identifier) op(unit) op(integer_literal) op(floating_point_literal) op(boolean_literal) op(string_literal) op(plus) op(increment) op(minus) op(decrement) op(star)          \
        op(slash) op(percent) op(assign) op(less_then) op(greather_then) op(exclamation_mark) op(logical_and) op(logical_or) op(equal) op(less_then_or_equal) op(greather_then_or_equal) op(not_equal) \
            op(transmutation_arrow) op(l_parentheses) op(r_parentheses) op(l_brace) op(r_brace) op(l_bracket) op(r_bracket) op(comma) op(semicolon) op(colon) op(double_colon) op(point) op(comment)   \
                op(eof) op(type_i32) op(type_f32) op(type_bool) op(type_str) op(keyword_as) op(keyword_if) op(keyword_else) op(keyword_while) op(keyword_return) op(keyword_pub) op(keyword_static)    \
                    op(keyword_type) op(keyword_extends) op(function_dump)

#define op(x) x,
enum class token_type
{
    TOKEN_TYPE_ENUMERATION(op)
};
#undef op

#define op(x)           \
    case token_type::x: \
        return #x;
static std::string to_string(token_type id_type)
{
    switch (id_type)
    {
        TOKEN_TYPE_ENUMERATION(op)
    default:
        return "undefined";
    }
};
#undef op

struct source_code_position
{
    int32_t line;
    int32_t inline_offset;

    friend std::ostream& operator<<(std::ostream& os, const source_code_position& position)
    {
        os << "[" << position.line << ", " << position.inline_offset << "]";
        return os;
    }
};

struct token
{
    token_type type = token_type::undefined;
    std::string text;
    source_code_position position = { 0, 0 };
};

static int32_t prefix_operator_precedence(token_type op)
{
    switch (op)
    {
    case token_type::plus:
        return 14;
    case token_type::minus:
        return 14;
    case token_type::exclamation_mark:
        return 14;
    case token_type::decrement:
        return 14;
    case token_type::increment:
        return 14;
    default:
        return -1;
    }
    return -1;
}

static int32_t postfix_operator_precedence(token_type op)
{
    switch (op)
    {
    case token_type::increment:
    case token_type::decrement:
    case token_type::l_bracket:
    case token_type::l_parentheses:
    case token_type::unit:
        return 15;
    default:
        return -1;
    }
    return -1;
}

static int32_t infix_operator_precedence(token_type op)
{
    switch (op)
    {
    case token_type::comma:
        return 0;
    case token_type::assign:
        return 1;
    case token_type::logical_or:
        return 2;
    case token_type::logical_and:
        return 3;
    case token_type::equal:
    case token_type::not_equal:
        return 6;
    case token_type::less_then:
    case token_type::greather_then:
    case token_type::less_then_or_equal:
    case token_type::greather_then_or_equal:
        return 7;
    case token_type::plus:
    case token_type::minus:
        return 11;
    case token_type::star:
    case token_type::slash:
    case token_type::percent:
        return 12;
    case token_type::keyword_as:
    case token_type::point:
        return 15;
    case token_type::double_colon:
        return 16;
    default:
        return -1;
    }
    return -1;
}

enum associativity
{
    LR = 0,
    RL = 1,
};

static int32_t infix_operator_associativity(token_type op)
{
    switch (op)
    {
    case token_type::plus:
    case token_type::minus:
    case token_type::star:
    case token_type::slash:
    case token_type::percent:
    case token_type::less_then:
    case token_type::greather_then:
    case token_type::logical_and:
    case token_type::logical_or:
    case token_type::equal:
    case token_type::less_then_or_equal:
    case token_type::greather_then_or_equal:
    case token_type::not_equal:
    case token_type::comma:
    case token_type::double_colon:
    case token_type::point:
    case token_type::keyword_as:
        return LR;
    case token_type::assign:
        return RL;
    default:
        return -1;
    }
    return -1;
}

/*
static int32_t operator_precedence(token_type op)
{
    switch (op)
    {
    case token_type::undefined:
        return 0;
    case token_type::token_identifier:
        return 0;
    case token_type::unit:
        return 0;
    case token_type::integer_literal:
        return 0;
    case token_type::floating_point_literal:
        return 0;
    case token_type::boolean_literal:
        return 0;
    case token_type::string_literal:
        return 0;
    case token_type::plus:
        return 0;
    case token_type::increment:
        return 0;
    case token_type::minus:
        return 0;
    case token_type::decrement:
        return 0;
    case token_type::star:
        return 0;
    case token_type::slash:
        return 0;
    case token_type::percent:
        return 0;
    case token_type::assign:
        return 0;
    case token_type::less_then:
        return 0;
    case token_type::greather_then:
        return 0;
    case token_type::exclamation_mark:
        return 0;
    case token_type::logical_and:
        return 0;
    case token_type::logical_or:
        return 0;
    case token_type::equal:
        return 0;
    case token_type::less_then_or_equal:
        return 0;
    case token_type::greather_then_or_equal:
        return 0;
    case token_type::not_equal:
        return 0;
    case token_type::transmutation_arrow:
        return 0;
    case token_type::l_parentheses:
        return 0;
    case token_type::r_parentheses:
        return 0;
    case token_type::l_brace:
        return 0;
    case token_type::r_brace:
        return 0;
    case token_type::l_bracket:
        return 0;
    case token_type::r_bracket:
        return 0;
    case token_type::comma:
        return 0;
    case token_type::semicolon:
        return 0;
    case token_type::colon:
        return 0;
    case token_type::double_colon:
        return 0;
    case token_type::point:
        return 0;
    case token_type::comment:
        return 0;
    case token_type::eof:
        return 0;
    case token_type::type_i32:
        return 0;
    case token_type::type_f32:
        return 0;
    case token_type::type_bool:
        return 0;
    case token_type::type_str:
        return 0;
    case token_type::keyword_as:
        return 0;
    case token_type::keyword_if:
        return 0;
    case token_type::keyword_else:
        return 0;
    case token_type::keyword_while:
        return 0;
    case token_type::keyword_return:
        return 0;
    case token_type::keyword_pub:
        return 0;
    case token_type::keyword_static:
        return 0;
    case token_type::keyword_type:
        return 0;
    case token_type::keyword_extends:
        return 0;
    case token_type::function_dump:
        return 0;

    default:
        break;
    }
}
*/

#endif TOKEN_HPP
