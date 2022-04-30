#include "lexer.hpp"
#include <iostream>

lexer::lexer()
{
    predefined["i32"]     = token_type::type_i32;
    predefined["f32"]     = token_type::type_f32;
    predefined["bool"]    = token_type::type_bool;
    predefined["str"]     = token_type::type_str;
    predefined["true"]    = token_type::boolean_literal;
    predefined["false"]   = token_type::boolean_literal;
    predefined["as"]      = token_type::keyword_as;
    predefined["if"]      = token_type::keyword_if;
    predefined["else"]    = token_type::keyword_else;
    predefined["while"]   = token_type::keyword_while;
    predefined["return"]  = token_type::keyword_return;
    predefined["pub"]     = token_type::keyword_pub;
    predefined["static"]  = token_type::keyword_static;
    predefined["type"]    = token_type::keyword_type;
    predefined["extends"] = token_type::keyword_extends;
    predefined["dump"]    = token_type::function_dump;
}

std::vector<token> lexer::parse(const std::string& source)
{
    std::vector<token> token_list;

    token current;
    int32_t line          = 1;
    int32_t inline_offset = 1;

    auto iter = source.cbegin();
    while (iter != source.cend())
    {
        inline_offset++;

        if (current.type == token_type::comment && !(*iter == '\n' || *iter == '\r'))
        {
            current.text.append(1, *iter);
            ++iter;
            continue;
        }
        if (current.type == token_type::string_literal && *iter != '"')
        {
            current.text.append(1, *iter);
            ++iter;
            continue;
        }

        switch (*iter)
        {
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case '_':
            if (current.type != token_type::token_identifier)
            {
                if (current.type == token_type::string_literal || current.type == token_type::integer_literal || current.type == token_type::floating_point_literal ||
                    current.type == token_type::boolean_literal)
                {
                    // continue and print more errors, if there are any
                    std::cerr << "Lexer: Unknown character \'" << *iter << "\' in " << current.text << " at (" << line << ", " << inline_offset << ")" << std::endl;
                }
                end_token(current, token_list);
                begin_token(current, line, inline_offset);
                current.type = token_type::token_identifier;
                current.text.append(1, *iter);
                scan_identifier(++iter, current);
                end_token(current, token_list);
            }
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::integer_literal;
            current.text.append(1, *iter);
            scan_number(++iter, current, line, inline_offset);
            end_token(current, token_list);
            break;
        case '"':
            if (current.type != token_type::string_literal)
            {
                end_token(current, token_list);
                begin_token(current, line, inline_offset);
                current.type = token_type::string_literal;
            }
            else
            {
                end_token(current, token_list);
            }
            break;
        case '+':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::plus;
            current.text.append(1, *iter);
            if (*(iter + 1) == '+') // ++ check
            {
                current.type = token_type::increment;
                current.text.append(1, *(++iter));
            }
            end_token(current, token_list);
            break;
        case '-':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::minus;
            current.text.append(1, *iter);
            if (*(iter + 1) == '-') // -- check
            {
                current.type = token_type::decrement;
                current.text.append(1, *(++iter));
            }
            else if (*(iter + 1) == '>') // -> check
            {
                current.type = token_type::transmutation_arrow;
                current.text.append(1, *(++iter));
            }
            end_token(current, token_list);
            break;
        case '*':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::star;
            current.text.append(1, *iter);
            end_token(current, token_list);
            break;
        case '/':
            end_token(current, token_list);
            if (*(iter + 1) == '/') // comment check
            {
                current.type = token_type::comment;
                iter += 2;
                continue;
            }
            begin_token(current, line, inline_offset);
            current.type = token_type::slash;
            current.text.append(1, *iter);
            end_token(current, token_list);
            break;
        case '%':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::percent;
            current.text.append(1, *iter);
            end_token(current, token_list);
            break;
        case '=':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::assign;
            current.text.append(1, *iter);
            if (*(iter + 1) == '=') // == check
            {
                current.type = token_type::equal;
                current.text.append(1, *(++iter));
            }
            end_token(current, token_list);
            break;
        case '<':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::less_then;
            current.text.append(1, *iter);
            if (*(iter + 1) == '=') // <= check
            {
                current.type = token_type::less_then_or_equal;
                current.text.append(1, *(++iter));
            }
            end_token(current, token_list);
            break;
        case '>':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::greather_then;
            current.text.append(1, *iter);
            if (*(iter + 1) == '=') // >= check
            {
                current.type = token_type::greather_then_or_equal;
                current.text.append(1, *(++iter));
            }
            end_token(current, token_list);
            break;
        case '!':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::exclamation_mark;
            current.text.append(1, *iter);
            if (*(iter + 1) == '=') // != check
            {
                current.type = token_type::not_equal;
                current.text.append(1, *(++iter));
            }
            end_token(current, token_list);
            break;
        case '&':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.text.append(1, *iter);
            if (*(iter + 1) == '&') // && check
            {
                current.type = token_type::logical_and;
                current.text.append(1, *(++iter));
            }
            else
            {
                // continue and print more errors, if there are any
                std::cerr << "Lexer: Unknown character \'" << *iter << "\' in " << current.text << " at (" << line << ", " << inline_offset << ")" << std::endl;
            }
            end_token(current, token_list);
            break;
        case '|':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.text.append(1, *iter);
            if (*(iter + 1) == '|') // && check
            {
                current.type = token_type::logical_or;
                current.text.append(1, *(++iter));
            }
            else
            {
                // continue and print more errors, if there are any
                std::cerr << "Unknown character \'" << *iter << "\' in " << current.text << " at (" << line << ", " << inline_offset << ")" << std::endl;
            }
            end_token(current, token_list);
            break;
        case '(':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::l_parentheses;
            current.text.append(1, *iter);
            if (*(iter + 1) == ')') // () check
            {
                current.type = token_type::unit;
                current.text.append(1, *(++iter));
            }
            end_token(current, token_list);
            break;
        case ')':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::r_parentheses;
            current.text.append(1, *iter);
            end_token(current, token_list);
            break;
        case '{':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::l_brace;
            current.text.append(1, *iter);
            end_token(current, token_list);
            break;
        case '}':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::r_brace;
            current.text.append(1, *iter);
            end_token(current, token_list);
            break;
        case '[':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::l_bracket;
            current.text.append(1, *iter);
            end_token(current, token_list);
            break;
        case ']':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::r_bracket;
            current.text.append(1, *iter);
            end_token(current, token_list);
            break;
        case ',':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::comma;
            current.text.append(1, *iter);
            end_token(current, token_list);
            break;
        case ';':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::semicolon;
            current.text.append(1, *iter);
            end_token(current, token_list);
            break;
        case '.':
            if (*(iter + 1) >= '0' && *(iter + 1) <= '9') // floating point check
            {
                end_token(current, token_list);
                begin_token(current, line, inline_offset);
                current.type = token_type::floating_point_literal;
                current.text.append(1, *iter);
                scan_number(++iter, current, line, inline_offset);
                end_token(current, token_list);
            }
            else
            {
                // member access operator
                end_token(current, token_list);
                begin_token(current, line, inline_offset);
                current.type = token_type::point;
                current.text.append(1, *iter);
                end_token(current, token_list);
            }
            break;
        case ':':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::colon;
            if (*(iter + 1) == ':') // :: check
            {
                current.type = token_type::double_colon;
                current.text.append(1, *(++iter));
            }
            current.text.append(1, *iter);
            end_token(current, token_list);
            break;
        case ' ':
        case '\t':
            end_token(current, token_list);
            break;
        case '\n':
            end_token(current, token_list);
            line++;
            inline_offset = 0;
            break;
        case '\r':
            end_token(current, token_list);
            line++;
            inline_offset = 0;
            if (*(iter + 1) == '\n') // \r\n check
                ++iter;
            break;
        default:
            // continue and print more errors, if there are any
            std::cerr << "Lexer: Unknown character \'" << *iter << "\' in " << current.text << " at (" << line << ", " << inline_offset << ")" << std::endl;
            break;
        }
        ++iter;
    }

    std::cout << std::endl;
    std::cout << "Source Info: " << line + 1 << " lines, " << source.length() << " characters." << std::endl;

    begin_token(current, line, inline_offset);
    current.type = token_type::eof;
    end_token(current, token_list);

    return token_list;
}

void lexer::begin_token(token& current, int32_t line, int32_t inline_offset)
{
    current.position = source_code_position{ line, inline_offset };
}

void lexer::end_token(token& current, std::vector<token>& token_list)
{
    if (current.type != token_type::undefined)
    {
        token_list.push_back(current);
    }

    current.type = token_type::undefined;
    current.text.erase();
    current.position = source_code_position{ -1, -1 };
}

void lexer::scan_identifier(std::string::const_iterator& iter, token& current)
{
    do
    {
        switch (*iter)
        {
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case '_':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            current.text.append(1, *iter);
            break;
        default:
            // character does not belong to identificator
            // check if identificator is predefined
            auto it = predefined.find(current.text);
            if (it != predefined.end())
            {
                current.type = it->second;
            }
            --iter;
            return;
        }
        ++iter;
    } while (1);
}

void lexer::scan_number(std::string::const_iterator& iter, token& current, int32_t line, int32_t& inline_offset)
{
    do
    {
        inline_offset++;
        switch (*iter)
        {
        case '.':
            if (current.type == token_type::floating_point_literal)
            {
                std::cerr << "Lexer: Unknown character \'" << *iter << "\' in " << current.text << " at (" << line << ", " << inline_offset << ")" << std::endl;
            }
            else if (*(iter + 1) >= '0' && *(iter + 1) <= '9')
            {
                current.type = token_type::floating_point_literal;
            }
            else
            {
                // continue and print more errors, if there are any
                std::cerr << "Lexer: Unknown character \'" << *iter << "\' in " << current.text << " at (" << line << ", " << inline_offset << ")" << std::endl;
            }
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            current.text.append(1, *iter);
            break;
        default:
            // chracter does not belong to number
            --iter;
            return;
        }
        ++iter;
    } while (1);
}