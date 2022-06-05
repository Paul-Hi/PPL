//! \file      parser.hpp
//! \author    Paul Himmler
//! \version   1.0
//! \date      2022
//! \copyright Apache License 2.0

#ifndef PARSER_HPP
#define PARSER_HPP

#include "ast.hpp"
#include "token.hpp"
#include <map>
#include <unordered_map>
#include <vector>

struct parser_error
{
    source_code_position position;
    std::string message;
};
using parser_error_list = std::vector<parser_error>;
using expected_result   = std::tuple<bool, token>;

#define IS_EXPECTED(t) std::get<0>(t)
#define EXPECTED_TOKEN(t) std::get<1>(t)

class parser_context
{
  public:
    parser_context()
        : temp_counter(0){};

    inline std::unique_ptr<identifier> declare_temp(source_code_position& position)
    {
        std::string name = "$I" + std::to_string(temp_counter++);

        unique_ptr<identifier> ident = std::make_unique<identifier>(position, name, identifier_type::undefined);

        return std::move(ident);
    }

  private:
    int32_t temp_counter;
};

class parser
{
  public:
    parser(const std::vector<token>& tokens)
        : m_tokens(tokens.rbegin(), tokens.rend()){};

    ~parser() = default;

    unique_ptr<expression> parse();

  private:
    // do not call after eof token was popped
    void pop_token()
    {
        m_tokens.pop_back();
    }
    void push_token(const token& t)
    {
        m_tokens.push_back(t);
    };
    // only call until eof token occurs
    token peek_token()
    {
        // last token always eof
        token t = m_tokens.back();
        if (t.type == token_type::comment)
        {
            do
            {
                pop_token();
                t = m_tokens.back();
            } while (t.type == token_type::comment);
        }
        return t;
    };
    // peeks and pops
    // only call until eof token occurs
    token next_token()
    {
        // last token always eof
        token t = peek_token();
        pop_token();
        return t;
    };

    void create_error(source_code_position& position, const std::string& msg);

    parser_context ctx;

    // peeks given token, consumes it if its type is equal to actual
    expected_result expect_token(token_type expected);

    unique_ptr<expression> parse_variable_definition(identifier_type id_type = identifier_type::undefined);
    unique_ptr<expression> parse_variable_declaration(identifier_type id_type = identifier_type::undefined);
    unique_ptr<expression> parse_parameter_declaration();
    unique_ptr<expression> parse_function_declaration();
    unique_ptr<expression> parse_type_name();
    unique_ptr<expression> parse_primitive_type_name();
    unique_ptr<expression> parse_function_type_name();
    unique_ptr<expression> parse_array_typename();
    unique_ptr<expression> parse_block();
    unique_ptr<expression> parse_identifier(identifier_type id_type = identifier_type::undefined);

    unique_ptr<expression> parse_statement();
    unique_ptr<expression> parse_expression_pratt(int32_t min_precedence = 0);
    unique_ptr<expression> parse_atom();

    unique_ptr<expression> add_prefix_op_expansion(source_code_position& position, token_type op, unique_ptr<expression>& rhs);
    unique_ptr<expression> add_postfix_op_expansion(source_code_position& position, token_type op, unique_ptr<expression>& rhs);

    std::vector<token> m_tokens;
    parser_error_list m_errors;
};

#endif PARSER_HPP
