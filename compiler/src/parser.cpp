#include "parser.hpp"
#include <iostream>

expected_result parser::expect_token(token_type expected)
{
    token t = peek_token();
    if (expected == t.type)
    {
        pop_token();
        return expected_result{ true, t };
    }

    create_error(t.position, "Missing " + to_string(expected) + " before " + to_string(t.type));
    return expected_result{ false, token() };
}

unique_ptr<expression> parser::parse()
{
    unique_ptr<expression> program = std::make_unique<expression>(source_code_position{ 0, 0 }, expression_type::compound);

    bool end = false;
    do
    {
        token t = peek_token();
        switch (t.type)
        {
        case token_type::unit:
        case token_type::l_parentheses:
        {
            // possible function declaration
            unique_ptr<expression> decl = parse_function_declaration();
            if (decl)
                program->expressions.push_back(std::move(decl));
            break;
        }
        case token_type::type_i32:
        case token_type::type_f32:
        case token_type::type_bool:
        case token_type::type_str:
        {
            // possible variable declaration
            unique_ptr<expression> decl = parse_variable_definition(identifier_type::variable);
            if (decl)
                program->expressions.push_back(std::move(decl));
            break;
        }
        case token_type::comment:
        case token_type::eof:
            pop_token();
            end = true;
            break;
        default:
            pop_token();
            create_error(t.position, "Unexpected token " + to_string(t.type));
            break;
        }
    } while (!end);

    for (parser_error err : m_errors)
    {
        std::cerr << err.message << err.position << std::endl;
        ;
    }

    return std::move(program);
}

unique_ptr<expression> parser::parse_variable_definition(identifier_type id_type)
{
    unique_ptr<expression> lvd;
    unique_ptr<expression> vd = parse_variable_declaration(id_type);

    token t = next_token();

    if (t.type == token_type::semicolon)
    {
        lvd = std::move(vd);
    }
    else if (t.type == token_type::assign)
    {
        unique_ptr<expression> lhs = parse_expression_pratt();
        lvd                        = std::make_unique<expression>(vd->source_position, expression_type::assign);
        lvd->expressions.push_back(std::move(vd));
        lvd->expressions.push_back(std::move(lhs));

        expect_token(token_type::semicolon);
    }
    else
    {
        create_error(t.position, "Unexpected token " + to_string(t.type));
    }

    return std::move(lvd);
}

unique_ptr<expression> parser::parse_variable_declaration(identifier_type id_type)
{
    unique_ptr<expression> tp_name = parse_type_name();

    if (!tp_name)
        return nullptr;

    auto expectation = expect_token(token_type::token_identifier);

    if (IS_EXPECTED(expectation))
        push_token(EXPECTED_TOKEN(expectation));
    else
        return nullptr;

    unique_ptr<expression> ident = parse_identifier(id_type);

    if (!ident)
        return nullptr;

    unique_ptr<expression> vd = std::make_unique<expression>(tp_name->source_position, expression_type::declaration);

    vd->expressions.push_back(std::move(tp_name));
    vd->expressions.push_back(std::move(ident));

    return std::move(vd);
}

unique_ptr<expression> parser::parse_parameter_declaration()
{
    token paren = peek_token();
    pop_token(); // (
    unique_ptr<expression> pd = std::make_unique<expression>(paren.position, expression_type::compound);

    do
    {
        token t = peek_token();
        switch (t.type)
        {
        case token_type::type_i32:
        case token_type::type_f32:
        case token_type::type_bool:
        case token_type::type_str:
        {
            // possible variable declaration
            unique_ptr<expression> decl = parse_variable_declaration(identifier_type::parameter);
            if (decl)
                pd->expressions.push_back(std::move(decl));

            token limiter = peek_token();

            switch (limiter.type)
            {
            case token_type::comma:
                pop_token(); // pop comma
                break;
            case token_type::r_parentheses:
                pop_token(); // pop paren
                return std::move(pd);
            default:
                create_error(limiter.position, "Unexpected token " + to_string(limiter.type));
                break;
            }
            break;
        }
        case token_type::r_parentheses:
            create_error(t.position, "Unexpected token " + to_string(t.type));
            return std::move(pd);
        default:
            create_error(t.position, "Unexpected token " + to_string(t.type));
            return std::move(pd);
        }
    } while (true);

    return std::move(pd);
}

unique_ptr<expression> parser::parse_function_declaration()
{
    unique_ptr<expression> function_tp_name = parse_function_type_name();

    if (!function_tp_name)
        return nullptr;

    token identifier_token = peek_token();

    auto expectation = expect_token(token_type::token_identifier);

    if (IS_EXPECTED(expectation))
        push_token(EXPECTED_TOKEN(expectation));
    else
        return nullptr;

    unique_ptr<expression> ident = parse_identifier(identifier_type::function);

    if (!ident)
        return nullptr;

    unique_ptr<expression> function_body = parse_block();

    if (!function_body)
        return nullptr;

    expect_token(token_type::r_brace);

    unique_ptr<expression> fd = std::make_unique<expression>(function_tp_name->source_position, expression_type::declaration);

    fd->expressions.push_back(std::move(function_tp_name));
    fd->expressions.push_back(std::move(ident));
    fd->expressions.push_back(std::move(function_body));

    return std::move(fd);
}

unique_ptr<expression> parser::parse_type_name()
{
    token type_name_token = next_token();

    token t = peek_token();

    if (t.type == token_type::l_bracket)
    {
        // at least 1D array
        push_token(type_name_token);
        unique_ptr<expression> atn = parse_array_typename();
        if (atn)
            return std::move(atn);
    }

    return std::move(std::make_unique<type_name>(type_name_token.position, type_name_token.text));
}

unique_ptr<expression> parser::parse_identifier(identifier_type id_type)
{
    token identifier_token = next_token();

    return std::move(std::make_unique<identifier>(identifier_token.position, identifier_token.text, id_type));
}

unique_ptr<expression> parser::parse_primitive_type_name()
{
    token type_name_token = next_token();

    return std::move(std::make_unique<type_name>(type_name_token.position, type_name_token.text));
}

unique_ptr<expression> parser::parse_function_type_name()
{
    token t = peek_token();

    unique_ptr<expression> ftn = std::make_unique<expression>(t.position, expression_type::function_type_name);

    if (t.type != token_type::unit)
    {
        // parse_arguments
        unique_ptr<expression> parameters = parse_parameter_declaration();
        ftn->expressions.push_back(std::move(parameters));
    }
    else
    {
        unique_ptr<expression> unit_type_name = parse_primitive_type_name();
        ftn->expressions.push_back(std::move(unit_type_name));
    }

    auto expectation = expect_token(token_type::transmutation_arrow);

    if (!IS_EXPECTED(expectation))
        return nullptr;

    token return_type_token = peek_token();
    unique_ptr<expression> return_type_name;

    switch (return_type_token.type)
    {
    case token_type::unit:
    case token_type::type_i32:
    case token_type::type_f32:
    case token_type::type_bool:
    case token_type::type_str:
        return_type_name = parse_type_name();
        if (!return_type_name)
            return nullptr;
        break;
    default:
        create_error(t.position, "Unexpected token " + to_string(t.type));
        break;
    }

    ftn->expressions.push_back(std::move(return_type_name));

    return std::move(ftn);
}

unique_ptr<expression> parser::parse_array_typename()
{
    token t = peek_token();

    unique_ptr<expression> atn = std::make_unique<expression>(t.position, expression_type::array_type_name);

    unique_ptr<expression> tn = parse_primitive_type_name();
    atn->expressions.push_back(std::move(tn));

    expect_token(token_type::l_bracket);

    unique_ptr<expression> size_expr = parse_expression_pratt();
    atn->expressions.push_back(std::move(size_expr));

    expect_token(token_type::r_bracket);

    token bracket = peek_token();
    while (bracket.type == token_type::l_bracket)
    {
        pop_token();

        unique_ptr<expression> size_expr = parse_expression_pratt();
        atn->expressions.push_back(std::move(size_expr));

        expect_token(token_type::r_bracket);
        bracket = peek_token();
    }

    return std::move(atn);
}

unique_ptr<expression> parser::parse_block()
{
    auto expectation = expect_token(token_type::l_brace);

    token l_brace_token = EXPECTED_TOKEN(expectation);

    unique_ptr<expression> block = std::make_unique<expression>(l_brace_token.position, expression_type::compound);

    do
    {
        token t = peek_token();
        switch (t.type)
        {
        case token_type::unit:
        case token_type::l_parentheses:
        {
            // possible function declaration
            unique_ptr<expression> decl = parse_function_declaration();
            if (decl)
                block->expressions.push_back(std::move(decl));
            break;
        }
        case token_type::type_i32:
        case token_type::type_f32:
        case token_type::type_bool:
        case token_type::type_str:
        {
            // possible variable declaration
            unique_ptr<expression> decl = parse_variable_definition(identifier_type::variable);
            if (decl)
                block->expressions.push_back(std::move(decl));
            break;
        }
        case token_type::keyword_while:
        case token_type::keyword_if:
        case token_type::keyword_return:
        case token_type::function_dump:
        case token_type::token_identifier:
        {
            // possible expression
            unique_ptr<expression> expr = parse_statement();
            if (expr)
                block->expressions.push_back(std::move(expr));
            break;
        }
        case token_type::l_brace:
        {
            // new block
            unique_ptr<expression> scope = parse_block();
            if (scope)
                block->expressions.push_back(std::move(scope));

            expect_token(token_type::r_brace);
            break;
        }
        case token_type::r_brace:
            return std::move(block);
        case token_type::eof:
            create_error(t.position, "Unexpected token " + to_string(t.type));
            return std::move(block);
        default:
            create_error(t.position, "Unexpected token " + to_string(t.type));
            return std::move(block);
        }
    } while (true);

    return std::move(block);
}

unique_ptr<expression> parser::parse_statement()
{
    token first_token = peek_token();

    unique_ptr<expression> expr = std::make_unique<expression>(first_token.position, expression_type::compound);

    switch (first_token.type)
    {
    case token_type::keyword_return:
    {
        // return statement
        expr->expr_type = expression_type::ret;
        pop_token();
        unique_ptr<expression> ret_expr = parse_expression_pratt();
        if (ret_expr)
            expr->expressions.push_back(std::move(ret_expr));
        expect_token(token_type::semicolon);
        break;
    }
    case token_type::keyword_if:
    {
        // if statement
        expr->expr_type = expression_type::branch;
        pop_token();
        unique_ptr<expression> condition = parse_expression_pratt();
        if (condition)
            expr->expressions.push_back(std::move(condition));
        // expect_token(token_type::r_parentheses); This is already checked by pratt parse
        unique_ptr<expression> then_block = parse_block();
        if (then_block)
            expr->expressions.push_back(std::move(then_block));
        expect_token(token_type::r_brace);
        if (peek_token().type == token_type::keyword_else)
        {
            pop_token();
            if (peek_token().type == token_type::l_brace)
            {
                unique_ptr<expression> else_block = parse_block();
                if (else_block)
                    expr->expressions.push_back(std::move(else_block));
                expect_token(token_type::r_brace);
            }
            else
            {
                unique_ptr<expression> else_if = parse_statement();
                if (else_if)
                    expr->expressions.push_back(std::move(else_if));
            }
        }
        break;
    }
    case token_type::keyword_while:
    {
        // while statement
        expr->expr_type = expression_type::loop;
        pop_token();
        unique_ptr<expression> guard = parse_expression_pratt();
        if (guard)
            expr->expressions.push_back(std::move(guard));
        // expect_token(token_type::r_parentheses); This is already checked by pratt parse
        unique_ptr<expression> block = parse_block();
        if (block)
            expr->expressions.push_back(std::move(block));
        expect_token(token_type::r_brace);
        break;
    }
    case token_type::function_dump:
        // dump call
    case token_type::token_identifier:
    {
        // function call or assignment
        unique_ptr<expression> fcall_or_assgn = parse_expression_pratt();
        if (fcall_or_assgn)
            expr = std::move(fcall_or_assgn);
        expect_token(token_type::semicolon);
        break;
    }
    default:
        create_error(first_token.position, "Unexpected token " + to_string(first_token.type));
        return std::move(expr);
    }

    return std::move(expr);
}

unique_ptr<expression> parser::parse_expression_pratt(int32_t min_precedence)
{
    token t = peek_token();
    unique_ptr<expression> lhs;
    int32_t prefix_prec = prefix_operator_precedence(t.type);
    if (prefix_prec >= 0)
    {
        // associativity is always associativity::RL;
        int32_t next_min_precedence = prefix_prec;

        pop_token();
        unique_ptr<expression> rhs = parse_expression_pratt(next_min_precedence);

        unique_ptr<expression> operation = std::make_unique<expression>(t.position, operator_token_type_to_expression_type(t.type));

        operation->expressions.push_back(std::move(lhs));
        operation->expressions.push_back(std::move(rhs));

        lhs = std::move(operation);
    }
    else
        lhs = parse_atom();

    if (!lhs)
        return nullptr;

    t = peek_token();

    while (true)
    {
        t = peek_token();

        int32_t postfix_prec = postfix_operator_precedence(t.type);
        if (postfix_prec >= 0)
        {
            if (postfix_prec < min_precedence)
                break;
            // associativity is always associativity::LR;
            int32_t next_min_precedence = postfix_prec + 1;

            pop_token();

            unique_ptr<expression> operation = std::make_unique<expression>(t.position, operator_token_type_to_expression_type(t.type));
            if (t.type == token_type::l_parentheses)
            {
                // function call
                unique_ptr<expression> rhs = parse_expression_pratt(0);
                expect_token(token_type::r_parentheses);
                operation->expressions.push_back(std::move(lhs));
                operation->expressions.push_back(std::move(rhs));
            }
            else if (t.type == token_type::l_bracket)
            {
                // array access
                unique_ptr<expression> rhs = parse_expression_pratt(0);
                expect_token(token_type::r_bracket);
                operation->expressions.push_back(std::move(lhs));
                operation->expressions.push_back(std::move(rhs));
            }
            else
            {
                operation->expressions.push_back(std::move(lhs));
            }

            lhs = std::move(operation);

            continue;
        }

        int32_t prec = infix_operator_precedence(t.type);
        if (prec < min_precedence)
            break;

        int32_t next_min_precedence = min_precedence;
        int32_t assoc               = infix_operator_associativity(t.type);
        if (assoc == associativity::LR)
            next_min_precedence = prec + 1;
        else
            next_min_precedence = prec;

        pop_token();
        unique_ptr<expression> rhs = parse_expression_pratt(next_min_precedence);

        unique_ptr<expression> operation = std::make_unique<expression>(lhs->source_position, operator_token_type_to_expression_type(t.type));
        operation->expressions.push_back(std::move(lhs));
        operation->expressions.push_back(std::move(rhs));

        lhs = std::move(operation);
    }

    return lhs;
}

unique_ptr<expression> parser::parse_atom()
{
    token atom_token = peek_token();

    switch (atom_token.type)
    {
    case token_type::l_parentheses:
    {
        pop_token();
        unique_ptr<expression> expr = parse_expression_pratt();
        expect_token(token_type::r_parentheses);
        return expr;
    }
    case token_type::function_dump:
    case token_type::token_identifier:
        return parse_identifier();
    case token_type::integer_literal:
        pop_token();
        return std::move(std::make_unique<integer_literal>(atom_token.position, std::stoi(atom_token.text)));
    case token_type::floating_point_literal:
        pop_token();
        return std::move(std::make_unique<floating_point_literal>(atom_token.position, std::stof(atom_token.text)));
    case token_type::string_literal:
        pop_token();
        return std::move(std::make_unique<string_literal>(atom_token.position, std::move(atom_token.text)));
    case token_type::boolean_literal:
        pop_token();
        return std::move(std::make_unique<boolean_literal>(atom_token.position, atom_token.text == "true"));
    case token_type::unit:
    case token_type::type_i32:
    case token_type::type_f32:
    case token_type::type_bool:
    case token_type::type_str:
        return parse_type_name();
    default:
        create_error(atom_token.position, "Unexpected token " + to_string(atom_token.type));
        return nullptr;
    }
}

void parser::create_error(source_code_position& position, const std::string& msg)
{
    m_errors.push_back(parser_error{ position, msg });
}
