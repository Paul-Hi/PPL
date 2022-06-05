//! \file      ast.hpp
//! \author    Paul Himmler
//! \version   1.0
//! \date      2022
//! \copyright Apache License 2.0

#ifndef AST_HPP
#define AST_HPP

#include "ast_visitor.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

template <typename T>
using unique_ptr = std::unique_ptr<T>;

class ast_node : public base_visitable<ast_node>
{
  public:
    virtual ~ast_node() = default;

    DEFINE_VISITABLE()

    source_code_position source_position;

    virtual std::string to_string()
    {
        return typeid(*this).name();
    }

  protected:
    ast_node(const source_code_position& position)
        : source_position(position)
    {
    }
};

#define IDENTIFIER_TYPE_ENUMERATION(op)              \
    op(undefined)     /* undefined */                \
        op(variable)  /* identifier for variables */ \
        op(function)  /* identifier for functions */ \
        op(parameter) /* identifier for parameters */

#define op(x) x,
enum class identifier_type
{
    IDENTIFIER_TYPE_ENUMERATION(op)
};
#undef op

#define op(x)                \
    case identifier_type::x: \
        return #x;
static std::string to_string(identifier_type id_type)
{
    switch (id_type)
    {
        IDENTIFIER_TYPE_ENUMERATION(op)
    default:
        return "unknown";
    }
};
#undef op

#define EXPRESSION_TYPE_ENUMERATION(op)                                                                                  \
    op(nop) op(str_lit) op(i32_lit) op(f32_lit) op(bool_lit) op(identifier)                /* atoms */                   \
        op(neg) op(add) op(sub) op(mult) op(div) op(mod)                                   /* arithmetic expressions */  \
        op(eq) op(neq) op(lt) op(gt) op(lte) op(gte) op(lnot) op(land) op(lor)             /* conditional expressions */ \
        op(assign) op(ret) op(branch) op(loop) op(cast) op(function_call) op(array_access) /* statements */              \
        op(type_name) op(array_type_name) op(function_type_name)                           /* type names */              \
        op(declaration) op(compound)                                                       /* blocks */

#define op(x) x,
enum class expression_type
{
    EXPRESSION_TYPE_ENUMERATION(op)
};
#undef op

static expression_type operator_token_type_to_expression_type(token_type op)
{
    switch (op)
    {
    case token_type::plus:
        return expression_type::add;
    case token_type::minus:
        return expression_type::sub;
    case token_type::star:
        return expression_type::mult;
    case token_type::slash:
        return expression_type::div;
    case token_type::percent:
        return expression_type::mod;
    case token_type::assign:
        return expression_type::assign;
    case token_type::less_then:
        return expression_type::lt;
    case token_type::greather_then:
        return expression_type::gt;
    case token_type::logical_and:
        return expression_type::land;
    case token_type::logical_or:
        return expression_type::lor;
    case token_type::equal:
        return expression_type::eq;
    case token_type::less_then_or_equal:
        return expression_type::lte;
    case token_type::greather_then_or_equal:
        return expression_type::gte;
    case token_type::not_equal:
        return expression_type::neq;
    case token_type::exclamation_mark:
        return expression_type::lnot;
    case token_type::comma:
        return expression_type::compound;
    case token_type::unit:
    case token_type::l_parentheses:
        return expression_type::function_call;
    case token_type::l_bracket:
        return expression_type::array_access;
    case token_type::keyword_as:
        return expression_type::cast;
    case token_type::double_colon:
    case token_type::point:
    default:
        return expression_type::compound;
    }
    return expression_type::compound;
}

class expression;

using expression_vector = std::vector<unique_ptr<expression>>;

class expression : public ast_node
{
  public:
    ~expression() = default;

    DEFINE_VISITABLE()

    expression(const source_code_position& position, expression_type expr_type)
        : ast_node(position)
        , expr_type(expr_type)

    {
    }

    expression()
        : ast_node(source_code_position{ 0, 0 })
        , expr_type(expression_type::nop)
    {
    }

    virtual unique_ptr<expression> deep_copy()
    {
        unique_ptr<expression> ret = std::make_unique<expression>(source_position, expr_type);

        for (auto& expr : expressions)
        {
            ret->expressions.push_back(std::move(expr->deep_copy()));
        }

        return ret;
    }

    expression_type expr_type;

    // program -> compound, expressions { all global expressions }
    // global_variable_declaration -> compound, expressions { type_name, identifier }
    // function_declaration -> compound, expressions { compound (function_type_name), identifier, compound (block) }
    // function_type_name -> compound, expressions { compound (parameter_declaration), type_name (return))}
    // block -> compound, expressions { all expressions in block }
    // function call -> function_call, expressions { args }
    // return -> ret, expressions { return expression }
    // if(a){b}else{c} -> branch, expressions {condition, block, (optional else block)}
    // while -> loop, expressions { guard, block }

    expression_vector expressions;

    bool is_pure() const
    {
        for (const auto& e : expressions)
            if (!e->is_pure())
                return false;
        switch (expr_type)
        {
        case expression_type::function_call: // not pure for now
            return false;
        case expression_type::assign: // not pure
            return false;
        case expression_type::ret: // not pure, does not evaluate into value
            return false;
        case expression_type::loop: // not pure, might be infinite
            return false;
        default:
            return true;
        }
    }

    std::string to_string() override
    {
        switch (expr_type)
        {
        case expression_type::neg:
            return "-";
        case expression_type::add:
            return "+";
        case expression_type::sub:
            return "-";
        case expression_type::mult:
            return "*";
        case expression_type::div:
            return "/";
        case expression_type::mod:
            return "%";
        case expression_type::eq:
            return "==";
        case expression_type::neq:
            return "!=";
        case expression_type::lt:
            return "<";
        case expression_type::gt:
            return ">";
        case expression_type::lte:
            return "<=";
        case expression_type::gte:
            return ">=";
        case expression_type::lnot:
            return "!";
        case expression_type::land:
            return "&&";
        case expression_type::lor:
            return "||";
        case expression_type::assign:
            return "=";
        case expression_type::ret:
            return "return";
        case expression_type::branch:
            return "branch";
        case expression_type::loop:
            return "loop";
        case expression_type::cast:
            return "as";
        case expression_type::function_call:
            return "fcall";
        case expression_type::array_access:
            return "[]";
        case expression_type::compound:
            return ",";
        case expression_type::declaration:
            return "decl";
        case expression_type::array_type_name:
            return "[]";
        case expression_type::function_type_name:
            return "->";
        case expression_type::nop:
        case expression_type::str_lit:
        case expression_type::i32_lit:
        case expression_type::f32_lit:
        case expression_type::bool_lit:
        case expression_type::identifier:
        case expression_type::type_name:
        default:
            return "";
            break;
        }
    }
};

class identifier : public expression
{
  public:
    identifier(const source_code_position& position, const std::string& name, identifier_type id_type)
        : expression(position, expression_type::identifier)
        , name(name)
        , id_type(id_type)
    {
    }
    ~identifier() = default;

    DEFINE_VISITABLE()

    std::string name;
    identifier_type id_type;

    unique_ptr<expression> deep_copy() override
    {
        std::string copy           = name;
        unique_ptr<expression> ret = std::make_unique<identifier>(source_position, std::move(copy), id_type);
        return ret;
    }

    std::string to_string() override
    {
        return name;
    }
};

class type_name : public expression
{
  public:
    type_name(const source_code_position& position, const std::string& string_representation)
        : expression(position, expression_type::type_name)
        , string_representation(string_representation)
    {
    }
    ~type_name() = default;

    DEFINE_VISITABLE()

    std::string string_representation;

    unique_ptr<expression> deep_copy() override
    {
        std::string copy           = string_representation;
        unique_ptr<expression> ret = std::make_unique<type_name>(source_position, std::move(copy));
        return ret;
    }

    std::string to_string() override
    {
        return string_representation;
    }
};

class integer_literal : public expression
{
  public:
    integer_literal(const source_code_position& position, int32_t value)
        : expression(position, expression_type::i32_lit)
        , value(value)
    {
    }
    ~integer_literal() = default;

    DEFINE_VISITABLE()

    int32_t value;

    unique_ptr<expression> deep_copy() override
    {
        unique_ptr<expression> ret = std::make_unique<integer_literal>(source_position, value);
        return ret;
    }

    std::string to_string() override
    {
        return std::to_string(value);
    }
};

class floating_point_literal : public expression
{
  public:
    floating_point_literal(const source_code_position& position, float value)
        : expression(position, expression_type::f32_lit)
        , value(value)
    {
    }
    ~floating_point_literal() = default;

    DEFINE_VISITABLE()

    float value;

    unique_ptr<expression> deep_copy() override
    {
        unique_ptr<expression> ret = std::make_unique<floating_point_literal>(source_position, value);
        return ret;
    }

    std::string to_string() override
    {
        return std::to_string(value);
    }
};

class string_literal : public expression
{
  public:
    string_literal(const source_code_position& position, std::string&& value)
        : expression(position, expression_type::f32_lit)
        , value(std::move(value))
    {
    }
    ~string_literal() = default;

    DEFINE_VISITABLE()

    std::string value;

    unique_ptr<expression> deep_copy() override
    {
        std::string copy           = value;
        unique_ptr<expression> ret = std::make_unique<string_literal>(source_position, std::move(copy));
        return ret;
    }

    std::string to_string() override
    {
        return "\\\"" + value + "\\\"";
    }
};

class boolean_literal : public expression
{
  public:
    boolean_literal(const source_code_position& position, bool value)
        : expression(position, expression_type::f32_lit)
        , value(value)
    {
    }
    ~boolean_literal() = default;

    DEFINE_VISITABLE()

    bool value;

    unique_ptr<expression> deep_copy() override
    {
        unique_ptr<expression> ret = std::make_unique<boolean_literal>(source_position, value);
        return ret;
    }

    std::string to_string() override
    {
        return value ? "true" : "false";
    }
};

class dot_visitor : public base_visitor<ast_node, void, int32_t&>
{
  public:
    dot_visitor(const std::string& out_file_name)
        : file(out_file_name)
    {
        DEFINE_VISITOR_AND_VISITABLES(dot_visitor, ast_node, expression);
        node_counter = 0;
        file << "\ndigraph G {\n";
        file << "\n  graph[ordering=\"out\"];\n";
    }
    ~dot_visitor()
    {
        file << "}\n";
    };

    void visit(ast_node& node, int32_t& param)
    {
        int32_t node_id = insert_node(node.to_string());
        param           = node_id;
    }

    void visit(expression& expr, int32_t& param)
    {
        int32_t node_id = insert_node(expr.to_string());
        visit_children(expr.expressions, node_id, param);
        param = node_id;
    }

  private:
    int32_t insert_node(const std::string& label)
    {
        int32_t node_id = node_counter++;
        file << "\n  node_" << node_id << " [label=\"" << label << "\"];\n";
        return node_id;
    }
    void visit_children(expression_vector& expressions, int32_t node_id, int32_t& param)
    {
        for (auto& child_expr : expressions)
        {
            if (!child_expr)
                continue;
            (*this)(*child_expr, param);
            file << "\n  node_" << node_id << " -> node_" << param << "\n";
        }
    }

    int32_t node_counter;
    std::ofstream file;
};

class pretty_printer : public base_visitor<ast_node, void, std::string>
{
  public:
    // TODO Next: Indentation!
    pretty_printer(const std::string& out_file_name)
        : file(out_file_name)
    {
        DEFINE_VISITOR_AND_VISITABLES(pretty_printer, ast_node, expression);
        file << "// Pretty printed ppl\n\n";
    }
    ~pretty_printer()
    {
        file << "\n\n// File end\n";
    };

    void visit(ast_node& node, std::string param)
    {
        file << node.to_string();
    }

    void visit(expression& expr, std::string param)
    {
        switch (expr.expr_type)
        {
        case expression_type::neg:
            file << expr.to_string();
            visit_children(expr.expressions, "");
            break;
        case expression_type::ret:
            file << expr.to_string() << " ";
            visit_children(expr.expressions, "");
            file << ";\n";
            break;
        case expression_type::add:
        case expression_type::sub:
        case expression_type::mult:
        case expression_type::div:
        case expression_type::mod:
        case expression_type::eq:
        case expression_type::neq:
        case expression_type::lt:
        case expression_type::gt:
        case expression_type::lte:
        case expression_type::gte:
        case expression_type::lnot:
        case expression_type::land:
        case expression_type::lor:
            visit_child(expr.expressions[0], "");
            file << " " << expr.to_string() << " ";
            visit_child(expr.expressions[1], "");
            break;
        case expression_type::assign:
            visit_child(expr.expressions[0], "");
            file << " " << expr.to_string() << " ";
            visit_child(expr.expressions[1], "");
            file << ";\n";
            break;
        case expression_type::branch:
            file << "if (";
            visit_child(expr.expressions[0], "");
            file << ")\n{\n";
            visit_child(expr.expressions[1], "");
            file << "}\n";
            if (expr.expressions.size() > 2)
            {
                file << "else";
                if (expr.expressions[2]->expr_type == expression_type::compound)
                {
                    file << "\n{\n";
                    visit_child(expr.expressions[2], "");
                    file << "}\n";
                }
                else
                {
                    file << " ";
                    visit_child(expr.expressions[2], "");
                }
            }
            break;
        case expression_type::loop:
            file << "while (";
            visit_child(expr.expressions[0], "");
            file << ")\n{\n";
            visit_child(expr.expressions[1], "");
            file << "}\n";
            break;
        case expression_type::cast:
            visit_child(expr.expressions[0], "");
            file << " " << expr.to_string() << " ";
            visit_child(expr.expressions[1], "");
            break;
        case expression_type::function_call:
            visit_child(expr.expressions[0], "");
            file << "(";
            if (expr.expressions.size() > 1)
                visit_child(expr.expressions[1], ", ");
            file << ")";
            break;
        case expression_type::array_access:
        {
            visit_child(expr.expressions[0], "");
            int32_t i = 1;
            while (expr.expressions.size() > i)
            {
                file << "[";
                visit_child(expr.expressions[i++], "");
                file << "]";
            }
            break;
        }
        case expression_type::type_name:
            break;
        case expression_type::array_type_name:
        {
            visit_child(expr.expressions[0], "");
            int32_t i = 1;
            while (expr.expressions.size() > i)
            {
                file << "[";
                visit_child(expr.expressions[i++], "");
                file << "]";
            }
            break;
        }
        case expression_type::function_type_name:
        {
            bool unit = !(expr.expressions[0]->expr_type == expression_type::compound || expr.expressions[0]->expr_type == expression_type::declaration);
            if (!unit)
                file << "(";
            visit_child(expr.expressions[0], ", ");
            if (!unit)
                file << ")";
            file << " " << expr.to_string() << " ";
            visit_child(expr.expressions[1], "");
            break;
        }
        case expression_type::compound:
            visit_children(expr.expressions, param);
            break;
        case expression_type::declaration:
            visit_child(expr.expressions[0], "");
            file << " ";
            visit_child(expr.expressions[1], "");
            if (expr.expressions.size() > 2)
            {
                file << "\n{\n";
                visit_child(expr.expressions[2], "");
                file << "}\n";
            }
            break;
        };
    }

  private:
    void visit_children(expression_vector& expressions, std::string param)
    {
        for (auto& child_expr : expressions)
        {
            if (!child_expr)
                continue;
            (*this)(*child_expr, "");
            if (expressions.back() != child_expr)
                file << param;
        }
    }
    void visit_child(unique_ptr<expression>& child_expr, std::string param)
    {
        if (!child_expr)
            return;
        (*this)(*child_expr, param);
    }

    std::ofstream file;
};
#endif AST_HPP